//
// Created by liu on 18-10-25.
//

#include "DumpTableQuery.h"
#include "../../db/Database.h"

#include <fstream>


QueryResult::Ptr DumpTableQuery::execute(TableIterator st, TableIterator ed) {
    // st and ed are meaningless here, simply pass something
    using namespace std;
    auto &db = Database::getInstance();
    try {
        ofstream outfile(this->fileName);
        if (!outfile.is_open()) {
            return make_unique<ErrorMsgResult>(queryName, "Cannot open file '?'"_f % this->fileName);
        }
        outfile << db[this->targetTable];
        outfile.close();
        return make_unique<SuccessMsgResult>(queryName, targetTable);
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(queryName, e.what());
    }
}

std::string DumpTableQuery::toString() {
    return "QUERY = Dump TABLE, FILE = \"" + this->fileName + "\"";
}


void DumpTableQuery::start() {
    Table::Iterator end = Database::getInstance()[this->targetTable].end();
    task = std::make_unique<Task>(end, end, this);
    ThreadPool::getInstance().addTask(task.get());
}

void DumpTableQuery::complete() {
    Database::getInstance().submit(this, std::move(task->result));
}