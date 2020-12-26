//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_QUITQUERY_H
#define PROJECT_QUITQUERY_H

#include "../QueryDerived.h"

class QuitQuery : public ManageQuery {
    std::unique_ptr<Task> task;
public:
    QuitQuery(): 
        ManageQuery("", "QUIT", true) {}

    QueryResult::Ptr execute(Table::Iterator st, Table::Iterator ed) override;

    std::string toString() override;

    void start() override;

    void complete() override;
};


#endif //PROJECT_QUITQUERY_H
