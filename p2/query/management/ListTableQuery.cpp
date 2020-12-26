//
// Created by liu on 18-10-25.
//

#include "ListTableQuery.h"
#include "../../db/Database.h"


QueryResult::Ptr ListTableQuery::execute(Table::Iterator st, Table::Iterator ed) {
    Database &db = Database::getInstance();
    db.printAllTable();
    return std::make_unique<SuccessMsgResult>(queryName);
}

std::string ListTableQuery::toString() {
    return "QUERY = LIST";
}

void ListTableQuery::start() {
    Table::Iterator end = Database::getInstance()[this->targetTable].end();
    task = std::make_unique<Task>(end, end, this);
    ThreadPool::getInstance().addTask(task.get());
}

void ListTableQuery::complete() {
    Database::getInstance().submit(this, std::move(task->result));
}