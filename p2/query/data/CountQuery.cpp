//
// Created by yuan on 20-10-24
//

#include "../../db/Database.h"
#include "CountQuery.h"


QueryResult::Ptr CountQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    // Ignore anything that exists in the ()
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = st; it != ed; ++it) {
                if (this->evalCondition(*it)) {
                    ++counter;
                }
            }
        }
        return make_unique<CountNumberResult>(counter);
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

std::unique_ptr<QueryResult> CountQuery::combine() {
  auto rq = std::make_unique<CountNumberResult>(0);
  for (auto &t: tasks) {
    rq->merge(std::move(t->result));
  }
  return rq;
}


std::string CountQuery::toString() {
    return "QUERY = Count " + this->targetTable + "\"";
}
