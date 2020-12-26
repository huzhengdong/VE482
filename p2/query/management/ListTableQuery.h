//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_LISTTABLEQUERY_H
#define PROJECT_LISTTABLEQUERY_H

#include "../QueryDerived.h"

class ListTableQuery : public ManageQuery {
    std::unique_ptr<Task> task;
public:
    ListTableQuery():
        ManageQuery("", "LIST", false) {}

    QueryResult::Ptr execute(Table::Iterator st, Table::Iterator ed) override;

    std::string toString() override;

    void start() override;

    void complete() override;
};


#endif //PROJECT_LISTTABLEQUERY_H
