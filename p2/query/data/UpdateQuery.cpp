//
// Created by liu on 18-10-25.
//

#include "UpdateQuery.h"
#include "../../db/Database.h"

QueryResult::Ptr UpdateQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
                queryName, this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        if (this->operands[0] == "KEY") {
            this->keyValue = this->operands[1];
        } else {
            this->fieldId = table.getFieldIndex(this->operands[0]);
            this->fieldValue = (Table::ValueType) strtol(this->operands[1].c_str(), nullptr, 10);
        }
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = st; it != ed; ++it) {
                if (this->evalCondition(*it)) {
                    if (this->keyValue.empty()) {
                        (*it)[this->fieldId] = this->fieldValue;
                    } else {
                        it->setKey(this->keyValue);
                    }
                    ++counter;
                }
            }
        }
        return make_unique<RecordCountResult>(counter);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, "Unkonwn error '?'."_f % e.what());
    }
}

std::unique_ptr<QueryResult> UpdateQuery::combine() {
  auto rq = std::make_unique<RecordCountResult>(0);
  // for each task, mergr its result into the rq.
  for (auto &t: tasks) {
    rq->merge(std::move(t->result));
  }
  return rq;
}

std::string UpdateQuery::toString() {
    return "QUERY = UPDATE " + this->targetTable + "\"";
}
