//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_INSERTQUERY_H
#define PROJECT_INSERTQUERY_H

#include "../QueryDerived.h"

/*
 * ** INSERT is special **
 * 1) it overwrites the start() & complete()
 * 2) it implmeneted combine and execute() as trivial method.
 * 3) it do not split
 */

class InsertQuery : public  ComplexQuery {
public:
    InsertQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition): ComplexQuery(targetTable, "INSERT", true, operands, condition) {}

    void complete() override;

    void start() override;

    virtual std::unique_ptr<QueryResult> combine() override { return nullptr; }

    virtual QueryResult::Ptr execute(TableIterator st, TableIterator ed) override {
      if (st != ed) return nullptr;
      else return nullptr;
    }

    QueryResult::Ptr execute();

    std::string toString() override;
};


#endif //PROJECT_INSERTQUERY_H
