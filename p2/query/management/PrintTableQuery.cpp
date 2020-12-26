//
// Created by liu on 18-10-25.
//

#include "PrintTableQuery.h"
#include "../../db/Database.h"

#include <iostream>

QueryResult::Ptr PrintTableQuery::execute(Table::Iterator st, Table::Iterator ed) {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        cout << "================\n";
        cout << "TABLE = ";
        cout << table;
        cout << "================\n" << endl;
        return make_unique<SuccessMsgResult>(queryName, this->targetTable);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, "No such table."s);
    }
}

std::string PrintTableQuery::toString() {
    return "QUERY = SHOWTABLE, Table = \"" + this->targetTable + "\"";
}

void PrintTableQuery::start() {
    Table::Iterator end = Database::getInstance()[this->targetTable].end();
    task = std::make_unique<Task>(end, end, this);
    ThreadPool::getInstance().addTask(task.get());
}

void PrintTableQuery::complete() {
    Database::getInstance().submit(this, std::move(task->result));
}