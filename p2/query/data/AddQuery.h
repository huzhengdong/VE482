//
// Created by lyy on 10/20/20.
//

#ifndef LEMONDB_ADDQUERY_H
#define LEMONDB_ADDQUERY_H

#include "../QueryDerived.h"

class AddQuery : public ComplexQuery {
public:
    Table::FieldIndex destid, srcid;
    AddQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
    ComplexQuery(targetTable, "ADD", true, operands, condition) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;

    std::string toString() override;
};

#endif //LEMONDB_ADDQUERY_H
