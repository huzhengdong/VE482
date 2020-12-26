//
// Created by liu on 18-10-25.
//

#include "LoadTableQuery.h"
#include "../../db/Database.h"

#include <fstream>

QueryResult::Ptr LoadTableQuery::execute(TableIterator st, TableIterator ed) {
    using namespace std;
    Database &db = Database::getInstance();
    try {
        // make table ready
        db[this->targetTable].fillTableWithFile(this->fileName);
        return make_unique<SuccessMsgResult>(queryName, targetTable);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(queryName, e.what());
    }
}

std::string LoadTableQuery::toString() {
    return "QUERY = Load TABLE, FILE = \"" + this->fileName + "\"";
}

void LoadTableQuery::start() {
    Table::Iterator end = Database::getInstance()[this->targetTable].end();
    task = std::make_unique<Task>(end, end, this);
    ThreadPool::getInstance().addTask(task.get());
}

void LoadTableQuery::complete() {
    Database::getInstance().submit(this, std::move(task->result));
}


