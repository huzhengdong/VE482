//
// Created by hzd on 2020/10/20.
//
#include "SumQuery.h"
#include "../../db/Database.h"

constexpr const char *SumQuery::qname;

QueryResult::Ptr SumQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    Table::ValueType counter = 0;
    Table::FieldIndex index = 0;

    Database &db = Database::getInstance();
    std::vector<Table::ValueType> answer (this->operands.size(),counter);
    std::vector<Table::FieldIndex> fieldId (this->operands.size(),index);
    if (ed == st)
        return make_unique<SumNumberResult>(answer);
    try {
        auto &table = db[this->targetTable];
        while(index < this->operands.size()) {
            fieldId[index] = table.getFieldIndex(this->operands[index]);
            index++;
        }
        auto result = initCondition(table);
        if (result.second) {
            for (auto it = st; it != ed; ++it) {
                if (this->evalCondition(*it)) {
                    index=0;
                    while(index < this->operands.size()) {
                        answer[index]+=(*it)[fieldId[index]];
                        index++;
                    }
                }
            }
        }
        return make_unique<SumNumberResult>(answer);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unkonwn error '?'."_f % e.what());
    }
}

std::unique_ptr<QueryResult> SumQuery::combine() {
Table::ValueType counter = 0;
std::vector<Table::ValueType> answer (this->operands.size(),counter);
auto rq = std::make_unique<SumNumberResult>(answer);
for (auto &t: tasks) {
        rq->merge(std::move(t->result));
    }
    return rq;
}


std::string SumQuery::toString() {
    return "QUERY = SUM " + this->targetTable + "\"";
}
