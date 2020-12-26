//
// Created by liu on 18-10-25.
//

#include "DropTableQuery.h"
#include "../../db/Database.h"


QueryResult::Ptr DropTableQuery::execute(TableIterator st, TableIterator ed) {
    // st and ed are meanningless here, simoply pass something
    using namespace std;
    Database &db = Database::getInstance();
    try {
        db[this->targetTable].clearAllData();
        return make_unique<SuccessMsgResult>(queryName);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(queryName, targetTable, "No such table."s);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(queryName, e.what());
    }
}

std::string DropTableQuery::toString() {
    return "QUERY = DROP, Table = \"" + targetTable + "\"";
}

void DropTableQuery::start() {
    Table::Iterator end = Database::getInstance()[this->targetTable].end();
    task = std::make_unique<Task>(end, end, this);
    ThreadPool::getInstance().addTask(task.get());
}

void DropTableQuery::complete() {
    Database::getInstance().submit(this, std::move(task->result));
}