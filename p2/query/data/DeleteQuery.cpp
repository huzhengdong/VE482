//
// Created by yuan on 20-10-24
//

#include "DeleteQuery.h"
#include "../../db/Database.h"
#include <iostream>


QueryResult::Ptr DeleteQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    // Ignore anything that exists in the ()
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        vector<Table::KeyType> keys;
        if (result.second) {
            for (auto it = st; it != ed; ++it) {
                if (this->evalCondition(*it)) {
                    keys.push_back(it->key());
                }
            }
        }
        return make_unique<DeleteQueryPartialResult>(std::move(keys));
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

std::unique_ptr<QueryResult> DeleteQuery::combine() {
  Table::SizeType c = 0;
  for (auto &t: tasks) {
    auto pq = dynamic_cast<DeleteQueryPartialResult *>(t->result.get());
    c += pq->size();
    Database::getInstance()[this->targetTable].erase(pq->v);
  }
  return std::make_unique<RecordCountResult>(c);

}

std::string DeleteQuery::toString() {
    return "QUERY = Delete " + this->targetTable + "\"";
}
