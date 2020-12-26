//
// Created by liu on 18-10-25.
//

#ifndef PROJECT_QUERY_DERIVED_H
#define PROJECT_QUERY_DERIVED_H

#include "QueryResult.h"
#include "../db/Table.h"
#include "../db/threadpool.h"
#include "Query.h"
#include "Task.h"


#include <functional>
#include <memory>
#include <string>
#include <mutex>


/*
 *  ThreadPool ---
 *      ^        |
 *      |        | a task finish
 *     task      |
 *      |  |------
 *      |  v        (self complete / all task complete)
 *     query -------------------------------------------> database ----> output stream          
 *      ^                                                     | 
 *      |                                                     |
 *      | start()                                             |
 *      |                                                     |
 *     Table <------------------------------------------------
 *                      notify
 */


struct QueryCondition {
    std::string field;
    size_t fieldId;
    std::string op;
    std::function<bool(const Table::ValueType &, const Table::ValueType &)> comp;
    std::string value;
    Table::ValueType valueParsed;
};

template <typename TaskType, typename ResultType>
class TaskIterationUtility {
  protected:
};

/*
 * All data queries will be splitable. a complex query will return 1 for method
 * isData()
 *
 * A ComplexQuery will splite the range into one or more subranges, 
 * and create tasks for those subranges.
 * When all tasks notify its completence, a complexQuery will notify
 * database its completence and pass the output to database.
 *
 * Any side effect for the tables will take place before the query notify 
 * database it completence
 *
 * What is implmeneted in this stage:
 * 1) how to start() (devide tasks and put it into pool)
 * 2) the complete() 
 *
 * TODO FOR EACH CHILD:
 * 1) overwrite execute method
 * 2) overwrite combine method
 *
 * ** INSERT is special **
 * 1) it overwrites the start() & complete()
 * 2) it implmeneted combine and execute() as trivial method.
 * 3) it do not split
 */
class ComplexQuery : public Query {
protected:
    // utilities to splite the query

    // taskNum, the number of tasks
    // taskFinish, the number of tasks finished
    size_t taskNum = 1, taskFinished = 0;
    // reference to tasks
    std::vector<std::unique_ptr<Task>> tasks;
    // mutex
    std::mutex taskMutex;

    // the final combined result
    std::unique_ptr<QueryResult> combineResult;

    // This method will merge the partial result and
    //
    // 1) generate an output
    // 2) apply changes to the targetTable
    virtual std::unique_ptr<QueryResult> combine() = 0;

    void complete() override;

    // This method take the table range and divide it to tasks
    // The tasks will be submitted to threadpool
    void start() override;

protected:
    /** The field names in the first () */
    std::vector<std::string> operands;
    /** The function used in where clause */
    std::vector<QueryCondition> condition;


public:
    typedef std::unique_ptr<ComplexQuery> Ptr;

    /**
     * init a fast condition according to the table
     * note that the condition is only effective if the table fields are not changed
     * @param table
     * @param conditions
     * @return a pair of the key and a flag
     * if flag is false, the condition is always false
     * in this situation, the condition may not be fully initialized to save time
     */
    std::pair<std::string, bool> initCondition(const Table &table);

    /**
     * skip the evaluation of KEY
     * (which should be done after initConditionFast is called)
     * @param conditions
     * @param object
     * @return
     */
    bool evalCondition(const Table::Object &object);

    /**
     * This function seems have small effect and causes somme bugs
     * so it is not used actually
     * @param table
     * @param function
     * @return
     */
    bool testKeyCondition(Table &table, std::function<void(bool, Table::Object::Ptr &&)> function);

    ComplexQuery(std::string targetTable, std::string queryName, bool isWrite,
                 std::vector<std::string> operands,
                 std::vector<QueryCondition> condition)
            : Query(std::move(targetTable), std::move(queryName), isWrite, true),
              operands(std::move(operands)),
              condition(std::move(condition)) {
    }

    /** Get operands in the query */
    const std::vector<std::string> &getOperands() const { return operands; }

    /** Get condition in the query, seems no use now */
    const std::vector<QueryCondition> &getCondition() { return condition; }
};

/*
 * All management Query is ManageQuery. It will return false when isData() is evoked.
 *
 * A management query can submit a single task to the threadpool.
 * Or it will not submit a task, but finish its job by itself (start() directly evoke complete()).
 */
class ManageQuery: public Query {
public:
  ManageQuery(std::string targetTable, std::string queryName, bool isWrite)
    : Query(std::move(targetTable), std::move(queryName), isWrite, false) {}

  void start() override {};

  void complete() override {};
};

#endif //PROJECT_QUERY_DERIVED_H
