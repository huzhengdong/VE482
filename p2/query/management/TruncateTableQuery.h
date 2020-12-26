//
//  TruncateTableQuery.h
//  LemonDB
//
//  Created by Francis on 2020/10/19.
//  Copyright © 2020 Francis. All rights reserved.
//

#ifndef TruncateTableQuery_h
#define TruncateTableQuery_h

#include "../QueryDerived.h"


class TruncateTableQuery : public ManageQuery {
    std::unique_ptr<Task> task;
public:
    TruncateTableQuery(std::string targetTable):
        ManageQuery(targetTable, "TRUNCATE", true) {}

    
    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;
    
    std::string toString() override;

    void start() override;

    void complete() override;
    
};

#endif /* TruncateTableQuery_h */
