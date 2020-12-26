//
//  TruncateTableQuery.cpp
//  LemonDB
//
//  Created by Francis on 2020/10/19.
//  Copyright © 2020 Francis. All rights reserved.
//

#include "TruncateTableQuery.h"
#include "../../db/Database.h"


std::string TruncateTableQuery::toString() {
    return "QUERY = TRUNCATET, Table = \"" + targetTable + "\"";
}

QueryResult::Ptr TruncateTableQuery::execute(TableIterator st, TableIterator ed) {
    // st and ed are meanningless, simply pass something
    using namespace std;
    Database &db = Database::getInstance();
    try {
        db.truncateTable(this->targetTable);
        return make_unique<SuccessMsgResult>(queryName);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(queryName, targetTable, "No such table."s);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(queryName, e.what());
    }
}

void TruncateTableQuery::start() {
    Table::Iterator end = Database::getInstance()[this->targetTable].end();
    task = std::make_unique<Task>(end, end, this);
    ThreadPool::getInstance().addTask(task.get());
}

void TruncateTableQuery::complete() {
    Database::getInstance().submit(this, std::move(task->result));
}