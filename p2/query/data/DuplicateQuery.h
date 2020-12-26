//
// Created by yuan on 2020-10-24
//

#ifndef DUPLICATEQUERY_H
#define DUPLICATEQUERY_H

#include "../QueryDerived.h"

class DuplicateQuery : public ComplexQuery {
public:
    DuplicateQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
      ComplexQuery(targetTable, "DUPLICATE", true, operands, condition) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;    

    std::string toString() override;
};

#endif /* DUPLICATEQUERY_H */
