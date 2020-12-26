//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_DUMPTABLEQUERY_H
#define PROJECT_DUMPTABLEQUERY_H

#include "../Query.h"
#include "../QueryDerived.h"

class DumpTableQuery : public ManageQuery {
    std::unique_ptr<Task> task;
    const std::string fileName;
public:
    DumpTableQuery(std::string table, std::string filename): 
        ManageQuery(table, "DUMP", false), fileName(std::move(filename)) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::string toString() override;

    void start() override;

    void complete() override;
};


#endif //PROJECT_DUMPTABLEQUERY_H
