//
// Created by liu on 18-10-23.
//
#include "Database.h"
#include "Table.h"
#include <deque>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <exception>
#include <unistd.h>
#include <stdio.h>

std::unique_ptr<Database> Database::instance = nullptr;

void Database::createEmptyTable(const std::string& tablename) {
    if (tables.find(tablename) != tables.end()) {
        return;
    }
    std::unique_ptr<Table> table_ptr = std::make_unique<Table>(tablename);
    // mark as not ready
    std::pair<std::string, Table::Ptr> pair = std::make_pair(tablename, std::move(table_ptr));
    tables.emplace(std::move(pair));
    return;
}

// from main add query op
void Database::addQuery(Query::Ptr query) {
    if (query->queryName == "QUIT") {
        //readyExit = true;  
        auto ref = query.get();
        resultsMutex.lock();
        this->queries.push(std::move(query));
        resultsMutex.unlock();
        ref->start();
        return;
    }
    if (query->queryName == "LISTEN") {
        resultsMutex.lock();
        this->queries.push(std::move(query));
        Query* ptr = this->queries.back().get();
        ListenQuery* t = dynamic_cast<ListenQuery*>(ptr);
        this->resultsMap[ptr] = std::make_unique<ListenQueryResult>(t->filename);
        resultsMutex.unlock();
        return;
    }
    if (query->queryName == "LOAD") {
        createEmptyTable(query->targetTable);
    } else if (query->queryName == "COPYTABLE") {
        // add this query to the target table queue for read control
        CopyTableQuery* cp_query = dynamic_cast<CopyTableQuery*>(query.get());
        createEmptyTable(cp_query->newTable);
    }
    auto ref = query.get();
    // push queries and results
    resultsMutex.lock();
    queries.push(std::move(query));
    resultsMap.emplace(ref, nullptr);
    resultsMutex.unlock();
    // add to table queue
    //std::cout << "??" << ref->queryName << "  target:  " << ref->targetTable << std::endl;
    Table& dstTable = this->operator[](ref->targetTable);
    //std::cout << "?? - end" << std::endl;
    dstTable.addQueryToTable(ref);
}

// complete one query: copy result, print result and notify table
void Database::submit(Query* query, QueryResult::Ptr result) {
    // notify table 
    if (query->targetTable != "") {
      auto targetTableIter = this->tables.find(query->targetTable);
      if (targetTableIter == this->tables.end()) {
          throw TableNameNotFound("Error when trying to drop table \"" + query->targetTable + "\". Table not found.");
      }
      targetTableIter->second->notifyQueryComplete();
    }
    // copy and print finished results:
    {
        resultsMutex.lock();
        // copy
        if (query->queryName != "QUIT") {
          auto res_iter = resultsMap.find(query);
          res_iter->second = std::move(result);
        }
        // print
        while (true) {
            //std::cout << "loop" << std::endl;
            if (queries.size() == 0) {
                //std::cout << "sz0" << std::endl;
                this->readyExit = finishParse;
                break;
            }
            Query* head = queries.front().get();
            // if quit mark as readyExit
            if (head->queryName == "QUIT") {
                //std::cout << "quit" << std::endl;
                this->readyExit = true;
                break;
            }
            // may throw
            auto tmp = resultsMap.find(head);
            if (tmp == resultsMap.end()) {
                // error handling
                throw std::runtime_error("ERROR: QUERY PTR NOT FOUND IN RESULTS MAP!");
            }
            if (tmp->second != nullptr) {
                //std::cout << "normal" << std::endl;
                std::cout << counter++ << std::endl;
                if (tmp->second->display()) {
                    std::cout << *(tmp->second);
                }
                // remove the one in resultsMap and in queries
                resultsMap.erase(tmp);
                queries.pop();
            } else {
                break;
            }
        }
        resultsMutex.unlock();
    }
}


void Database::testDuplicate(const std::string &tableName) {
    auto it = this->tables.find(tableName);
    if (it != this->tables.end()) {
        throw DuplicatedTableName(
                "Error when inserting table \"" + tableName + "\". Name already exists."
        );
    }
}

Table &Database::registerTable(Table::Ptr &&table) {
    auto name = table->name();
    this->testDuplicate(table->name());
    auto result = this->tables.emplace(name, std::move(table));
    return *(result.first->second);
}


Table &Database::operator[](const std::string &tableName) {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end()) {
        throw TableNameNotFound(
                "Error accesing table \"" + tableName + "\". Table not found."
        );
    }
    return *(it->second);
}

const Table &Database::operator[](const std::string &tableName) const {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end()) {
        throw TableNameNotFound(
                "Error accesing table \"" + tableName + "\". Table not found."
        );
    }
    return *(it->second);
}

void Database::dropTable(const std::string &tableName) {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end()) {
        throw TableNameNotFound(
                "Error when trying to drop table \"" + tableName + "\". Table not found."
        );
    }
    this->tables.erase(it);
}

void Database::truncateTable(const std::string& tableName) {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end()) {
        throw TableNameNotFound(
                "Error when trying to truncate table \"" + tableName + "\". Table not found."
        );
    }
    it->second->clear();
}

// @deprecated
void Database::copyTable(const std::string& tableName, const std::string& newTable) {
    auto it = this->tables.find(tableName);
    if (it == this->tables.end()) {
        throw TableNameNotFound(
                "Error when trying to copy table \"" + tableName + "\". Table not found."
        );
    }
    const Table& srcTable = *(it->second);
    auto table = std::make_unique<Table>(newTable, srcTable);
    registerTable(std::move(table));
}

void Database::printAllTable() {
    const int width = 15;
    std::cout << "Database overview:" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << std::setw(width) << "Table name";
    std::cout << std::setw(width) << "# of fields";
    std::cout << std::setw(width) << "# of entries" << std::endl;
    for (const auto &table : this->tables) {
        std::cout << std::setw(width) << table.first;
        std::cout << std::setw(width) << (*table.second).field().size() + 1;
        std::cout << std::setw(width) << (*table.second).size() << std::endl;
    }
    std::cout << "Total " << this->tables.size() << " tables." << std::endl;
    std::cout << "=========================" << std::endl;
}

Database &Database::getInstance() {
    if (Database::instance == nullptr) {
        instance = std::unique_ptr<Database>(new Database);
    }
    return *instance;
}

void Database::updateFileTableName(const std::string &fileName, const std::string &tableName) {
    fileTableNameMap[fileName] = tableName;
}

std::string Database::getFileTableName(const std::string &fileName) {
    auto it = fileTableNameMap.find(fileName);
    if (it == fileTableNameMap.end()) {
    //char cwd[1024];
   // getcwd(cwd, sizeof(cwd));
    //std::cout << "Current working dir: " << cwd << std::endl;
        std::ifstream infile(fileName);
        if (!infile.is_open()) {
            return "";
        }
        std::string tableName;
        infile >> tableName;
       // std::cout << tableName << std::endl;
     //   while (infile) {
      //    std::string o;
       //   infile >> o;
        //  std::cout << o << std::endl;
       // }
        infile.close();
        fileTableNameMap.emplace(fileName, tableName);
        return tableName;
    } else {
        return it->second;
    }
}

// @deprecated
Table &Database::loadTableFromStream(std::istream &is, std::string source) {
    auto &db = Database::getInstance();
    std::string errString =
            !source.empty() ?
            R"(Invalid table (from "?") format: )"_f % source :
            "Invalid table format: ";

    std::string tableName;
    Table::SizeType fieldCount;
    std::deque<Table::KeyType> fields;

    std::string line;
    std::stringstream sstream;
    if (!std::getline(is, line))
        throw LoadFromStreamException(
                errString + "Failed to read table metadata line."
        );

    sstream.str(line);
    sstream >> tableName >> fieldCount;
    if (!sstream) {
        throw LoadFromStreamException(
                errString + "Failed to parse table metadata."
        );
    }

    // throw error if tableName duplicates
    db.testDuplicate(tableName);

    if (!(std::getline(is, line))) {
        throw LoadFromStreamException(
                errString + "Failed to load field names."
        );
    }

    sstream.clear();
    sstream.str(line);
    for (Table::SizeType i = 0; i < fieldCount; ++i) {
        std::string field;
        if (!(sstream >> field)) {
            throw LoadFromStreamException(
                    errString + "Failed to load field names."
            );
        }
        else {
            fields.emplace_back(std::move(field));
        }
    }

    if (fields.front() != "KEY") {
        throw LoadFromStreamException(
                errString + "Missing or invalid KEY field."
        );
    }

    fields.erase(fields.begin()); // Remove leading key
    auto table = std::make_unique<Table>(tableName, fields);

    Table::SizeType lineCount = 2;
    while (std::getline(is, line)) {
        if (line.empty()) break; // Read to an empty line
        lineCount++;
        sstream.clear();
        sstream.str(line);
        std::string key;
        if (!(sstream >> key))
            throw LoadFromStreamException(
                    errString + "Missing or invalid KEY field."
            );
        std::vector<Table::ValueType> tuple;
        tuple.reserve(fieldCount - 1);
        for (Table::SizeType i = 1; i < fieldCount; ++i) {
            Table::ValueType value;
            if (!(sstream >> value))
                throw LoadFromStreamException(
                        errString + "Invalid row on LINE " + std::to_string(lineCount)
                );
            tuple.emplace_back(value);
        }
        table->insertByIndex(key, std::move(tuple));
    }

    return db.registerTable(std::move(table));
}


void Database::exit() {
    // We are being lazy here ...
    // Might cause problem ...
    std::exit(0);
}

void Database::markFinishParse() {
    resultsMutex.lock();
    finishParse = true;
    resultsMutex.unlock();
}
