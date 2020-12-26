//
// Created by liu on 18-10-25.
//

#include "../../db/Database.h"
#include "InsertQuery.h"
#include "../QueryResult.h"

#include <algorithm>

QueryResult::Ptr InsertQuery::execute() {
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(
                queryName, this->targetTable.c_str(),
                "No operand (? operands)."_f % operands.size()
        );
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        auto &key = this->operands.front();
        vector<Table::ValueType> data;
        data.reserve(this->operands.size() - 1);
        for (auto it = ++this->operands.begin(); it != this->operands.end(); ++it) {
            data.emplace_back(strtol(it->c_str(), nullptr, 10));
        }
        table.insertByIndex(key, move(data));
        return std::make_unique<SuccessMsgResult>(queryName, targetTable);
    }
    catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
        // Cannot convert operand to string
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(queryName, this->targetTable, "Unkonwn error '?'."_f % e.what());
    }
}

void InsertQuery::complete() {
  auto r = execute();
  Database::getInstance().submit(this, std::move(r));
}

void InsertQuery::start() {
  complete();
}

std::string InsertQuery::toString() {
    return "QUERY = INSERT " + this->targetTable + "\"";
}
