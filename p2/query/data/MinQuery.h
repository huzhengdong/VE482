//
// Created by 13 on 2020/10/21.
//

#ifndef PGROUP_01_MINQUERY_H
#define PGROUP_01_MINQUERY_H
#include "../QueryDerived.h"
#include "../multi_thread.h"

class MinQuery : public ComplexQuery {
    //std::vector<Table::ValueType> answer;
    //std::vector<Table::FieldIndex> fieldId;
public:
    MinQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
      ComplexQuery(targetTable, "MIN", false, operands, condition) {}
        

    QueryResult::Ptr execute(Table::Iterator st, Table::Iterator ed) override;

    std::string toString() override;

    std::unique_ptr<QueryResult> combine() override;
};


#endif //PGROUP_01_MINQUERY_H
