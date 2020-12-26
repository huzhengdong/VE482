#ifndef DELETEQUERY_H
#define DELETEQUERY_H

#include "../QueryDerived.h"

class DeleteQuery : public ComplexQuery {
public:
    DeleteQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
      ComplexQuery(targetTable, "DELETE", true, operands, condition) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;    

    std::string toString() override;
};

#endif /* DELETEQUERY_H */
