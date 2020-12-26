//
// Created by lyy on 11/22/20.
//
#include "ListenQuery.h"
#include "../../db/Database.h"

QueryResult::Ptr ListenQuery::execute(Table::Iterator st, Table::Iterator ed) {
    return std::make_unique<ListenQueryResult>(filename);
}

std::string ListenQuery::toString() {
    return "QUERY = LISTEN";
}

void ListenQuery::start() {
}

void ListenQuery::complete() {
}


