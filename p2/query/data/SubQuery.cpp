//
// Created by lyy on 10/20/20.
//

#include "SubQuery.h"
#include "../../db/Database.h"

QueryResult::Ptr SubQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    if (this->operands.size() < 2)
        return make_unique<ErrorMsgResult>(
                queryName, this->targetTable.c_str(),
                "Not enough operands (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        this->srcid=table.getFieldIndex(this->operands[0]);
        int size=this->operands.size()-1;
        this->destid=table.getFieldIndex(this->operands[size]);

        if (result.second) {
            for (auto it = st; it != ed; ++it) {
                if (this->evalCondition(*it)) {
                    auto res = (*it)[srcid];
                    for(int i=1;i<size;i++){
                        auto id=table.getFieldIndex(this->operands[i]);
                        res-=(*it)[id];
                    }
                    (*it)[destid]=res;
                    ++counter;
                }
            }
        }
        return make_unique<AffectNumberResult>(counter);
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

std::unique_ptr<QueryResult> SubQuery::combine() {
    auto rq = std::make_unique<AffectNumberResult>(0);
    for (auto &t: tasks) {
        rq->merge(std::move(t->result));
    }
    return rq;
}

std::string SubQuery::toString() {
    return "QUERY = SUB " + this->targetTable + "\"";
}

