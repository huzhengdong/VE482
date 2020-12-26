cmake_minimum_required(VERSION 2.7)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-DNDEBUG -Wall -Wextra -Werror -pedantic -Wno-unused-result -Wno-unused-parameter -O2 -g")

set(SOURCE_FILES
        db/Database.cpp db/Database.h
        db/Table.cpp db/Table.h
        db/threadpool.cpp db/threadpool.h

        utils/formatter.h utils/uexception.h

        query/Query.h
        query/QueryDerived.cpp query/QueryDerived.h
        query/Task.h
		query/QueryParser.cpp query/QueryParser.h
        query/QueryResult.h query/QueryResult.cpp
        query/QueryBuilders.cpp query/QueryBuilders.h

        query/management/LoadTableQuery.cpp query/management/LoadTableQuery.h
        query/management/DropTableQuery.cpp query/management/DropTableQuery.h
        query/management/DumpTableQuery.cpp query/management/DumpTableQuery.h
		query/management/ListTableQuery.cpp query/management/ListTableQuery.h
	    query/management/TruncateTableQuery.cpp query/management/TruncateTableQuery.h
	    query/management/CopyTableQuery.cpp query/management/CopyTableQuery.h
        query/management/QuitQuery.cpp query/management/QuitQuery.h
        query/management/PrintTableQuery.cpp query/management/PrintTableQuery.h
		query/management/ListenQuery.cpp query/management/ListenQuery.h

        query/data/InsertQuery.cpp query/data/InsertQuery.h
        query/data/UpdateQuery.cpp query/data/UpdateQuery.h
        query/data/DeleteQuery.cpp query/data/DeleteQuery.h
        query/data/DuplicateQuery.cpp query/data/DuplicateQuery.h
        query/data/CountQuery.cpp query/data/CountQuery.h
	    query/data/SumQuery.cpp query/data/SumQuery.h
        query/data/MaxQuery.cpp query/data/MaxQuery.h
        query/data/MinQuery.cpp query/data/MinQuery.h
        query/data/SelectQuery.cpp query/data/SelectQuery.h
        query/data/SwapQuery.h query/data/SwapQuery.cpp
        query/data/AddQuery.cpp query/data/AddQuery.h
        query/data/SubQuery.cpp query/data/SubQuery.h
		query/multi_thread.cpp query/multi_thread.h)
add_executable(lemondb main.cpp ${SOURCE_FILES})
target_link_libraries(lemondb pthread)

