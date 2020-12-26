//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_DROPTABLEQUERY_H
#define PROJECT_DROPTABLEQUERY_H

#include "../QueryDerived.h"

class DropTableQuery : public ManageQuery {
    std::unique_ptr<Task> task;
public:
    DropTableQuery(std::string table): 
        ManageQuery(table, "DROP", true) {}

    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::string toString() override;

    void start() override;

    void complete() override;
};


#endif //PROJECT_DROPTABLEQUERY_H
