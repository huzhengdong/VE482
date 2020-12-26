//
// Created by 13 on 2020/10/20.
//

#include "MaxQuery.h"
#include "../../db/Database.h"


constexpr const char *MaxQuery::qname;

QueryResult::Ptr MaxQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    Table::ValueType counter = Table::ValueTypeMin;
    Table::FieldIndex index = 0;
    bool has = false;
    Database &db = Database::getInstance();

    std::vector<Table::ValueType> answer (this->operands.size(),counter);
    std::vector<Table::FieldIndex> fieldId (this->operands.size(),index);
    if (this->operands.size() ==0 )
        return make_unique<MaxNumberResult>(has, answer);


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
                    has = true;
                    index=0;
                    while(index < this->operands.size()) {
                        answer[index]=(answer[index]>(*it)[fieldId[index]])?answer[index]:(*it)[fieldId[index]];
                        index++;
                    }
                }
            }
            //if (has)
               // return make_unique<RecordMessageResult>(answer);
            //else
               // return make_unique<NullQueryResult>();
        }
        //else
        return make_unique<MaxNumberResult>(has, answer);
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

std::unique_ptr<QueryResult> MaxQuery::combine() {
    bool has=false;
    Table::ValueType counter = Table::ValueTypeMin;
    std::vector<Table::ValueType> answer (this->operands.size(),counter);
    auto rq = std::make_unique<MaxNumberResult>(has, answer);
    for (auto &t: tasks) {
        rq->merge(std::move(t->result));
    }
    return rq;
}



std::string MaxQuery::toString() {
    return "QUERY = MAX " + this->targetTable + "\"";
}
