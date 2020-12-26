//
// Created by liu on 18-10-23.
//

#ifndef PROJECT_DB_H
#define PROJECT_DB_H

#include <memory>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <fstream>

#include "Table.h"
#include "../query/QueryResult.h"
#include "../query/Query.h"
#include "../query/Task.h"
#include "../query/management/CopyTableQuery.h"
#include "../query/management/ListenQuery.h"


class Database {
private:
    /**
     * A unique pointer to the global database object
     */
    static std::unique_ptr<Database> instance;

    /**
     * The map of tableName -> table unique ptr
     */
    std::unordered_map<std::string, Table::Ptr> tables;

    /**
     * The map of fileName -> tableName
     */
    std::unordered_map<std::string, std::string> fileTableNameMap;

    /**
     * The default constructor is made private for singleton instance
     */
    Database() = default;

    std::queue<Query::Ptr> queries;
    
    std::unordered_map<Query*, QueryResult::Ptr> resultsMap;
    
    std::mutex resultsMutex;

    size_t counter = 1;

public:
    bool readyExit = false;

    bool finishParse = false;

    void markFinishParse();

    void createEmptyTable(const std::string& tablename);

    void addQuery(Query::Ptr query);

    void submit(Query* query, QueryResult::Ptr result);

    void testDuplicate(const std::string &tableName);

    Table &registerTable(Table::Ptr &&table);

    void dropTable(const std::string &tableName);
    
    void truncateTable(const std::string& tableName);
    
    void copyTable(const std::string& tableName, const std::string& newTable);

    void printAllTable();

    Table &operator[](const std::string &tableName);

    const Table &operator[](const std::string &tableName) const;

    Database &operator=(const Database &) = delete;

    Database &operator=(Database &&) = delete;

    Database(const Database &) = delete;

    Database(Database &&) = delete;

    ~Database() = default;

    static Database &getInstance();

    void updateFileTableName(const std::string &fileName, const std::string &tableName);

    std::string getFileTableName(const std::string &fileName);

    /**
     * Load a table from an input stream (i.e., a file)
     * @param is
     * @param source
     * @return reference of loaded table
     */
    // @deprecated
    Table &loadTableFromStream(std::istream &is, std::string source = "");

    void exit();
};


#endif //PROJECT_DB_H
