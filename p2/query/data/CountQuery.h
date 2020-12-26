//
// Created by yuan on 2020-10-24
//

#ifndef COUNTQUERY_H
#define COUNTQUERY_H

#include "../QueryDerived.h"

class CountQuery : public ComplexQuery {
public:
    CountQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
      ComplexQuery(targetTable, "COUNT", false, operands, condition) {}
    
    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;    

    std::string toString() override;
};

#endif /* COUNTQUERY_H */
