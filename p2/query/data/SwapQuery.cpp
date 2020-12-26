//
// Created by lyy on 10/20/20.
//

#include "SwapQuery.h"
#include "../../db/Database.h"

QueryResult::Ptr SwapQuery::execute(TableIterator st, TableIterator ed) {
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
        this->field1 = table.getFieldIndex(this->operands[0]);
        this->field2 = table.getFieldIndex(this->operands[1]);
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = st; it !=ed; ++it) {
                if (this->evalCondition(*it)) {
                    Table::ValueType temp = (*it)[this->field1];
                    (*it)[this->field1] = (*it)[this->field2];
                    (*it)[this->field2] = temp;
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

std::unique_ptr<QueryResult> SwapQuery::combine() {
    auto rq = std::make_unique<AffectNumberResult>(0);
    for (auto &t: tasks) {
        rq->merge(std::move(t->result));
    }
    return rq;
}

std::string SwapQuery::toString() {
    return "QUERY = SWAP " + this->targetTable + "\"";
}
