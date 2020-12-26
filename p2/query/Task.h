#ifndef PROJECT_TASK_H
#define PROJECT_TASK_H

#include "Query.h"


/*
 * A task contains a partial query's configuration
 * and a place (a reference) for the result. Every 
 * Query will have one or more tasks.
 *
 * A task will use the computation method (execute) to 
 * compute a partial result. The result would be referenced 
 * by a pointer inside.
 *
 * A task's unique_ptr will be kept in a query
 */
struct Task {
  using TableIterator = Query::TableIterator;
  TableIterator st, ed;
  Query *qptr;

  std::unique_ptr<QueryResult> result;

  Task(TableIterator begin, TableIterator end, Query *q): st(begin), ed(end), qptr(q) {}

  void execute() {
    result = qptr->execute(st, ed);
    qptr->complete();
  }
};

#endif
