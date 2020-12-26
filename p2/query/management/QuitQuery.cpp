//
// Created by liu on 18-10-25.
//

#include "QuitQuery.h"
#include "../../db/Database.h"


std::string QuitQuery::toString() {
    return "QUERY = Quit";
}

QueryResult::Ptr QuitQuery::execute(Table::Iterator st, Table::Iterator ed) {
//    auto &db = Database::getInstance();
//    db.exit();
    // might not reach here, but we want to keep the consistency of queries
    return std::make_unique<SuccessMsgResult>(queryName);
}

void QuitQuery::start() {
    complete();
}

void QuitQuery::complete() {
    auto r = std::make_unique<SuccessMsgResult>(queryName);
    Database::getInstance().submit(this, std::move(r));
}
