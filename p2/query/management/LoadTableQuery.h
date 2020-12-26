//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_LOADTABLEQUERY_H
#define PROJECT_LOADTABLEQUERY_H


#include "../QueryDerived.h"

class LoadTableQuery : public ManageQuery {
    const std::string fileName;
    std::unique_ptr<Task> task;
public:
    explicit LoadTableQuery(std::string table, std::string fileName) :
            ManageQuery(table, "LOAD", true), fileName(std::move(fileName)) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::string toString() override;

    void start() override;

    void complete() override;

};


#endif //PROJECT_LOADTABLEQUERY_H
