//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_PRINTTABLEQUERY_H
#define PROJECT_PRINTTABLEQUERY_H

#include "../QueryDerived.h"

class PrintTableQuery : public ManageQuery {
    std::unique_ptr<Task> task;
public:
    PrintTableQuery(std::string targetTable):
        ManageQuery(targetTable, "SHOWTABLE", false) {}

    QueryResult::Ptr execute(Table::Iterator st, Table::Iterator ed) override;

    std::string toString() override;

    void start() override;

    void complete() override;
};

#endif //PROJECT_PRINTTABLEQUERY_H
