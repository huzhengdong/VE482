//
//  CopyTableQuery.cpp
//  LemonDB
//
//  Created by Francis on 2020/10/19.
//  Copyright © 2020 Francis. All rights reserved.
//

#include "CopyTableQuery.h"
#include "../../db/Database.h"


std::string CopyTableQuery::toString() {
    return "QUERY = COPY TABLE, FROM TABLE = \"" + targetTable + "\" TO TABLE = \"" + this->newTable + "\"";
}

QueryResult::Ptr CopyTableQuery::execute(TableIterator st, TableIterator ed) {
    // st and ed are meanningless simply pass something
    using namespace std;
    Database &db = Database::getInstance();
    try {
        Table& dst = db[this->newTable];
        Table& src = db[this->targetTable];
        dst.copyDataFrom(src);
        dst.makeReady();
        dst.notifyQueryComplete();
        return make_unique<SuccessMsgResult>(queryName, targetTable);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(queryName, e.what());
    }
}

void CopyTableQuery::start() {
    Table::Iterator end = Database::getInstance()[this->targetTable].end();
    task = std::make_unique<Task>(end, end, this);
    ThreadPool::getInstance().addTask(task.get());
}

void CopyTableQuery::complete() {
    Database::getInstance().submit(this, std::move(task->result));
}
