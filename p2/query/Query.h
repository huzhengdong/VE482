#ifndef PROJECT_QUERY_H
#define PROJECT_QUERY_H

#include "QueryResult.h"
#include "../db/Table.h"
/*
 * a query is the finest unit visible by the database
 *
 * If database thinks a query is safe to be run at whatever later time,
 * it should call start()
 *
 * Once a query is finished in some later time, it will notify the database
 * by calling its method <methodname>, from its method complete().
 *
 * REQUIREMENT:
 * a query should be free to read or write to its targetTable before it
 * notifies database its completence.
 *
 * EFFECT:
 * potentially change the content of a targetTable
 * generate a output, which use information of a targetTable
 */
class Query {
    friend struct Task;
protected:
    using TableIterator = Table::Iterator;
    const bool isWrite_b = 0;
    const bool isData_b = 0;

public:
    int id = -1;
    const std::string targetTable;
    const std::string queryName;
    Query() = delete;

public:
    explicit Query(std::string targetTable, std::string queryName, bool isWrite, bool isData) : isWrite_b(isWrite), isData_b(isData), targetTable(std::move(targetTable)), queryName(std::move(queryName)) {}

    bool isWrite() { return isWrite_b; }
    bool isData() { return isData_b; }

    typedef std::unique_ptr<Query> Ptr;


    /* do the compute tasks for the Query, should be used by the tasks. Can change the content of table if it is necessary */
    virtual QueryResult::Ptr execute(TableIterator st, TableIterator ed) = 0;

    /* put tasks into threadpool, overrided by each query */
    virtual void start() = 0;

    /* a task or a trivival query use this method to signal the query it has completed.
     *
     * a query, once received this signal, will either wait other tasks or go to the commit phase.
     *
     * In commit phase, all tasks complete and their result has been gathered.
     * 1) if the query needs to change the tables content based on the computatation result, change it.
     * 2) signal database it has completed and commit the output to it.
     */
    virtual void complete() = 0;

    virtual std::string toString() = 0;

    virtual ~Query() = default;
};

#endif
