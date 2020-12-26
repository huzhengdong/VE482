//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_UPDATEQUERY_H
#define PROJECT_UPDATEQUERY_H

#include "../QueryDerived.h"

class UpdateQuery : public ComplexQuery {
    Table::ValueType fieldValue;// = (operands[0]=="KEY")? 0 :std::stoi(operands[1]);
    Table::FieldIndex fieldId;
    Table::KeyType keyValue;
public:
    UpdateQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
      ComplexQuery(targetTable, "UPDATE", true, operands, condition) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;

    std::string toString() override;
};


#endif //PROJECT_UPDATEQUERY_H
