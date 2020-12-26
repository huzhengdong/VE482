//
// Created by lyy on 10/20/20.
//

#ifndef LEMONDB_SWAPQUERY_H
#define LEMONDB_SWAPQUERY_H

#include "../QueryDerived.h"

class SwapQuery : public ComplexQuery {
public:
    Table::FieldIndex field1,field2;
    SwapQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
        ComplexQuery(targetTable, "SWAP", true, operands, condition) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;

    std::string toString() override;
};


#endif //LEMONDB_SWAPQUERY_H
