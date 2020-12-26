//
// Created by lyy on 11/22/20.
//

#ifndef LEMONDB_LISTENQUERY_H
#define LEMONDB_LISTENQUERY_H

#include "../QueryDerived.h"

class ListenQuery : public ManageQuery {
public:
    std::string filename;
    ListenQuery(std::string filename):
            ManageQuery("", "LISTEN", false),filename(filename) {}

    QueryResult::Ptr execute(Table::Iterator st, Table::Iterator ed) override;

    std::string toString() override;

    void start() override;

    void complete() override;
};

#endif //LEMONDB_LISTENQUERY_H
