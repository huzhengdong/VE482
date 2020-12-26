//
// Created by liu on 18-10-23.
//
// Modified by yuan on 20-10-24:
// [DeleteQuery]
// 1. add the definition of erase()
// 2. include <algorithm>

#include "Database.h"
#include "Table.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <queue>

constexpr const Table::ValueType Table::ValueTypeMax;
constexpr const Table::ValueType Table::ValueTypeMin;

Table::FieldIndex Table::getFieldIndex(const Table::FieldNameType &field) const {
    try {
        return this->fieldMap.at(field);
    } catch (const std::out_of_range &e) {
        throw TableFieldNotFound(
                R"(Field name "?" doesn't exists.)"_f % (field)
        );
    }
}

void Table::insertByIndex(KeyType key, std::vector<ValueType> &&data) {
    if (this->keyMap.find(key) != this->keyMap.end()) {
        std::string err = "In Table \"" + this->tableName
                          + "\" : Key \"" + key + "\" already exists!";
        throw ConflictingKey(err);
    }
    this->keyMap.emplace(key, this->data.size());
    this->data.emplace_back(std::move(key), data);
}

void Table::insertByAppend(std::vector<Datum> &&v) {
  Table::SizeType i = data.size(); 
  for (auto &k : v) {
    keyMap.emplace(k.key, i++);
  }
  data.insert(data.end(), std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
}

void Table::erase(const std::vector<KeyType> &keys) {
  auto end = data.size();//, endc = end;
  for (const auto &k: keys) {
    try {
      auto pos = keyMap[k]; 
      keyMap[data[--end].key] = pos;
      std::swap(data[pos], data[end]);
      keyMap.erase(k);
    } catch (const std::exception &e) {
      continue;
    }
  }
  data.resize(end);
} 

Table::Object::Ptr Table::operator[](const Table::KeyType &key) {
    auto it = keyMap.find(key);
    if (it == keyMap.end()) {
        // not found
        return nullptr;
    } else {
        return createProxy(data.begin() + it->second, this);
    }
}

std::ostream &operator<<(std::ostream &os, const Table &table) {
    const int width = 10;
    std::stringstream buffer;
    buffer << table.tableName << "\t" << (table.fields.size() + 1) << "\n";
    buffer << std::setw(width) << "KEY";
    for (const auto &field : table.fields) {
        buffer << std::setw(width) << field;
    }
    buffer << "\n";
    auto numFields = table.fields.size();
    for (const auto &datum : table.data) {
        buffer << std::setw(width) << datum.key;
        for (decltype(numFields) i = 0; i < numFields; ++i) {
            buffer << std::setw(width) << datum.datum[i];
        }
        buffer << "\n";
    }
    return os << buffer.str();
}
void Table::addQueryToTable(Query *query) {
    queueMutex.lock();
    if(isReady)
    {
        //if the queue is empty
        if(queryQueue.empty())
        {
            //if the query is a writer
            if(query->isWrite())
            {
                if(ReadwriteSem==0)
                {
                    ReadwriteSem--;
                    queueMutex.unlock();
                    query->start();
                }
                else{
                    queryQueue.push_back(query);
                    queueMutex.unlock();
                }
            }
                //if the query is a reader
            else{
                if(ReadwriteSem>=0)
                {
                    ReadwriteSem++;
                    queueMutex.unlock();
                    query->start();
                }
                else{
                    queryQueue.push_back(query);
                    queueMutex.unlock();
                }
            }

        }
            //if the query is not empty
        else{
            queryQueue.push_back(query);
            queueMutex.unlock();
        }

    }
    else{
        //if the table is not initilised and the queue is empty, execute immediately
        // possible situation: loading or not loaded, copying
        if (query->queryName == "LOAD" && queryQueue.empty())
        {
            // not loaded
            ReadwriteSem = -1;
            queueMutex.unlock();
            query->start();
        }
        else{
            queryQueue.push_back(query);
            queueMutex.unlock();
        }
    }


}
void Table::notifyQueryComplete(){
    queueMutex.lock();
    if(isReady)
    {
        //if the queue is not empty
        if(ReadwriteSem<=0)
            ReadwriteSem=0;
            //if the last query completes reading
        else
            ReadwriteSem--;
        if(!queryQueue.empty())
        {
            decltype(queryQueue) readPool;

            // readWriteSem >= 0
            if(ReadwriteSem==0)
            {
                auto query = queryQueue.front();
                if(query->isWrite())
                {
                    ReadwriteSem --;
                    queryQueue.pop_front();
                    queueMutex.unlock();
                    query->start();
                    return;
                }
            }
            //Not empty and not allowed to write readWriteSem must >= 0
            while (!queryQueue.empty() && !queryQueue.front()->isWrite()) {
                auto query = queryQueue.front();
                queryQueue.pop_front();
                ReadwriteSem++;
                readPool.push_back(query);
            }
            queueMutex.unlock();
            for(auto &read: readPool)
                read->start();
            return;
        }
        //if the query is  empty
        else{
            queueMutex.unlock();
            return;
        }
    } else {
        // not ready, only has load/copy
        if (!queryQueue.empty() && !queryQueue.front()->isData()) {
            auto query = queryQueue.front();
            queryQueue.pop_front();
            queueMutex.unlock();
            query->start();
            return;
        }
        else{
            queueMutex.unlock();
            return;
        }
    }
}

void Table::makeReady() {
    queueMutex.lock();
    isReady= true;
    queueMutex.unlock();
}

