//
// Created by liu on 18-10-25.
//

#include "QueryDerived.h"
#include "../db/Database.h"

#include <cassert>

void ComplexQuery::complete()  {
      taskMutex.lock();
      if (++taskFinished < taskNum) {
        // if not all tasks finished
        taskMutex.unlock();
        return;
      }
      taskMutex.unlock();
      
      // [commit phase] all task finished
      combineResult = combine();
      Database::getInstance().submit(this, std::move(combineResult));
}

void ComplexQuery::start()  {
     Table &table(Database::getInstance()[targetTable]);
     TableIterator begin = table.begin();
     auto size = table.size();

     taskMutex.lock(); // get the lock
     // use a good looking 10000 as step
     const int step = 10000;
     taskNum = (size == 0) ? 1 : (size - 1) / step + 1;

     // for the first (taskNum - 1) tasks
     // The range is full 10000
     for (size_t i = 0; i < taskNum - 1; i++) {
       auto bg = begin + i * step;
       auto ed = bg + step;
       auto t = std::unique_ptr<Task>(new Task(bg, ed, this));
       auto tt = t.get();
       tasks.emplace_back(std::move(t));
       ThreadPool::getInstance().addTask(tt);
     }
     // the remaining last task use table end
     // if the size is 0, a trivial task is submitted (bg = ed)
     auto bg = begin + (taskNum - 1) * step;
     auto ed = table.end();
     auto t = std::unique_ptr<Task>(new Task(bg, ed, this));
     auto tt = t.get();
     tasks.emplace_back(std::move(t));
     ThreadPool::getInstance().addTask(tt);
     taskMutex.unlock();
}

std::pair<std::string, bool>
ComplexQuery::initCondition(const Table &table) {
    const std::unordered_map<std::string, int> opmap{
            {">",  '>'},
            {"<",  '<'},
            {"=",  '='},
            {">=", 'g'},
            {"<=", 'l'},
    };
    std::pair<std::string, bool> result = {"", true};
    for (auto &cond : condition) {
        if (cond.field == "KEY") {
            if (cond.op != "=") {
                throw IllFormedQueryCondition(
                        "Can only compare equivalence on KEY"
                );
            } else if (result.first.empty()) {
                result.first = cond.value;
            } else if (result.first != cond.value) {
                result.second = false;
                return result;
            }
        } else {
            cond.fieldId = table.getFieldIndex(cond.field);
            cond.valueParsed = (Table::ValueType) std::strtol(cond.value.c_str(), nullptr, 10);
            int op = 0;
            try {
                op = opmap.at(cond.op);
            } catch (const std::out_of_range &e) {
                throw IllFormedQueryCondition(
                        R"("?" is not a valid condition operator.)"_f % cond.op
                );
            }
            switch (op) {
                case '>' :
                    cond.comp = std::greater<>();
                    break;
                case '<' :
                    cond.comp = std::less<>();
                    break;
                case '=' :
                    cond.comp = std::equal_to<>();
                    break;
                case 'g' :
                    cond.comp = std::greater_equal<>();
                    break;
                case 'l' :
                    cond.comp = std::less_equal<>();
                    break;
                default:
                    assert(0); // should never be here
            }
        }
    }
    return result;
}

bool ComplexQuery::evalCondition(const Table::Object &object) {
    bool ret = true;
    for (const auto &cond : condition) {
        if (cond.field != "KEY") {
            ret = ret && cond.comp(object[cond.fieldId], cond.valueParsed);
        } else {
            ret = ret && (object.key() == cond.value);
        }
    }
    return ret;
}

bool ComplexQuery::testKeyCondition(Table &table, std::function<void(bool, Table::Object::Ptr &&)> function) {
    auto condResult = initCondition(table);
    if (!condResult.second) {
        function(false, nullptr);
        return true;
    }
    if (!condResult.first.empty()) {
        auto object = table[condResult.first];
        if (object != nullptr && evalCondition(*object)) {
            function(true, std::move(object));
        } else {
            function(false, nullptr);
        }
        return true;
    }
    return false;
}
