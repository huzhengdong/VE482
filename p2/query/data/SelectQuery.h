//
//  SelectQuery.h
//  LemonDB
//
//  Created by Francis on 2020/10/19.
//  Copyright © 2020 Francis. All rights reserved.
//

#ifndef SelectQuery_h
#define SelectQuery_h

#include "../QueryDerived.h"

class SelectQuery : public ComplexQuery {
public:
    SelectQuery(std::string targetTable, std::vector<std::string> operands, std::vector<QueryCondition> condition) :
      ComplexQuery(targetTable, "SELECT", false, operands, condition) {}
    
    QueryResult::Ptr execute(TableIterator st, TableIterator ed) override;

    std::unique_ptr<QueryResult> combine() override;    

    std::string toString() override;
};


#endif /* SelectQuery_h */
