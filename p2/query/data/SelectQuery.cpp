//
//  SelectQuery.cpp
//  LemonDB
//
//  Created by Francis on 2020/10/19.
//  Copyright © 2020 Francis. All rights reserved.
//

#include "SelectQuery.h"
#include "../../db/Database.h"

std::string SelectQuery::toString() {
    return "QUERY = SELECT " + this->targetTable + "\"";
}

QueryResult::Ptr SelectQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    if (this->operands[0] != "KEY") {
        return make_unique<ErrorMsgResult>(
                                           queryName,
                                           this->targetTable.c_str(),
                                           "First operand must be KEY."
                                           );
    }
    // this->operands;
    const vector<string>& operands = this->operands;
    Database &db = Database::getInstance();
    vector<vector<string>> results;
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = st; it != ed; ++it) {
                if (this->evalCondition(*it)) {
                    vector<string> tmp;
                    // push back key
                    tmp.push_back(it->key());
                    // push back value
                    for_each(operands.begin()+1, operands.end(), [&it, &tmp](const string& f){
                        tmp.push_back(to_string(it->get(f)));
                    });
                    results.push_back(move(tmp));
                }
            }
        }
        return make_unique<RecordSelectResult>(results);
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

std::unique_ptr<QueryResult> SelectQuery::combine() {
  std::unique_ptr<RecordSelectResult> rq = std::make_unique<RecordSelectResult>();
  // for each task, mergr its result into the rq.
  for (auto &t: tasks) {
    rq->merge(std::move(t->result));
  }
  rq->finish();
  return rq;
}
