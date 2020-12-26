//
// Created by lyy on 10/20/20.
//

#ifndef LEMONDB_SUBQUERY_H
#define LEMONDB_SUBQUERY_H

#include "../QueryDerived.h"

class SubQuery : public ComplexQuery {
public:
    Table::FieldIndex destid, srcid;
    SubQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
    ComplexQuery(targetTable, "SUB", true, operands, condition) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;

    std::string toString() override;
};

#endif //LEMONDB_SUBQUERY_H
