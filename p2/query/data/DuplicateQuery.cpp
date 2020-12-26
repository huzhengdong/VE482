//
// Created by yuan on 20-10-24
//

#include "DuplicateQuery.h"
#include "../../db/Database.h"
#include <iostream>

QueryResult::Ptr DuplicateQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    // Ignore anything that exists in the ()
    Database &db = Database::getInstance();
    std::vector<Table::Datum> v;
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = st; it != ed; ++it) {
                if (this->evalCondition(*it)) {
                  if (!table.existKey(it->key() + "_copy")) {
                    v.emplace_back(it->key() + "_copy", it->copyValue());
                  }
               }
            }
        }
        return make_unique<DuplicateQueryTaskResult>(std::move(v));
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

std::unique_ptr<QueryResult> DuplicateQuery::combine() {
  Table::SizeType c = 0;
  for (auto &t: tasks) {
    auto pq = dynamic_cast<DuplicateQueryTaskResult *>(t->result.get());
    c += pq->size();
    Database::getInstance()[this->targetTable].insertByAppend(std::move(pq->v));
  }
  return std::make_unique<RecordCountResult>(c);
}

std::string DuplicateQuery::toString() {
    return "QUERY = Duplicate " + this->targetTable + "\"";
}
