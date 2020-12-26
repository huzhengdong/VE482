//
//  CopyTableQuery.h
//  LemonDB
//
//  Created by Francis on 2020/10/19.
//  Copyright © 2020 Francis. All rights reserved.
//

#ifndef CopyTableQuery_h
#define CopyTableQuery_h

#include "../Query.h"
#include "../QueryDerived.h"

class CopyTableQuery : public ManageQuery {
    std::unique_ptr<Task> task;
public: 
    const std::string newTable;

    CopyTableQuery(std::string targetTable, std::string newTable):
        ManageQuery(targetTable, "COPYTABLE", false), newTable(newTable) {}
    
    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;
    
    std::string toString() override;

    void start() override;

    void complete() override;
    
};

#endif /* CopyTableQuery_h */
