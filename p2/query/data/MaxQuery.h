//
// Created by 13 on 2020/10/20.
//

#ifndef PGROUP_01_MAXQUERY_H
#define PGROUP_01_MAXQUERY_H
#include "../Query.h"
#include "../QueryDerived.h"

class MaxQuery : public ComplexQuery {
    static constexpr const char *qname = "MAX";
public:
    MaxQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
    ComplexQuery(targetTable, "COUNT", false, operands, condition) {}
    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;
    std::unique_ptr<QueryResult> combine() override;
    std::string toString() override;
};


#endif //PGROUP_01_MAXQUERY_H
