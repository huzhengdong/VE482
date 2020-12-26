//
// Created by liu on 18-10-21.
//

#include "query/QueryParser.h"
#include "query/QueryBuilders.h"
#include "query/multi_thread.h"
#include "db/Database.h"

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stack>


struct {
    std::string listen;
    long threads = 2;
} parsedArgs;

void parseArgs(int argc, char *argv[]) {
    const option longOpts[] = {
            {"listen",  required_argument, nullptr, 'l'},
            {"threads", required_argument, nullptr, 't'},
            {nullptr,   no_argument,       nullptr, 0}
    };
    const char *shortOpts = "l:t:";
    int opt, longIndex;
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, &longIndex)) != -1) {
        if (opt == 'l') {
            parsedArgs.listen = optarg;
        } else if (opt == 't') {
            parsedArgs.threads = std::strtol(optarg, nullptr, 10);
        } else {
            std::cerr << "lemondb: warning: unknown argument " << longOpts[longIndex].name << std::endl;
        }
    }

}

std::string extractQueryString(std::istream &is) {
    std::string buf;
    do {
//        std::cout << "read " << std::endl;
        int ch = is.get();
//        std::cout << "ch: " << ch << std::endl;
        if (ch == ';') return buf;
        if (ch == EOF) {
         // std::cout << " eof" << std::endl;
          throw std::ios_base::failure("End of input");
        }
        buf.push_back((char) ch);
    } while (true);
}

int main(int argc, char *argv[]) {
    // Assume only C++ style I/O is used in lemondb
    // Do not use printf/fprintf in <cstdio> with this line
    std::ios_base::sync_with_stdio(false);

    parseArgs(argc, argv);

    std::ifstream fin;
    if (!parsedArgs.listen.empty()) {
        fin.open(parsedArgs.listen);
        if (!fin.is_open()) {
            std::cerr << "lemondb: error: " << parsedArgs.listen << ": no such file or directory" << std::endl;
            exit(-1);
        }
    }
    std::istream is(fin.rdbuf());

#ifdef NDEBUG
    // In production mode, listen argument must be defined
    if (parsedArgs.listen.empty()) {
        std::cerr << "lemondb: error: --listen argument not found, not allowed in production mode" << std::endl;
        exit(-1);
    }
#else
    // In debug mode, use stdin as input if no listen file is found
    if (parsedArgs.listen.empty()) {
        std::cerr << "lemondb: warning: --listen argument not found, use stdin instead in debug mode" << std::endl;
        is.rdbuf(std::cin.rdbuf());
    }
#endif

    if (parsedArgs.threads < 0) {
        std::cerr << "lemondb: error: threads num can not be negative value " << parsedArgs.threads << std::endl;
        exit(-1);
    } else if (parsedArgs.threads == 0) {
        // @TODO Auto detect the thread num
        std::cerr << "lemondb: info: auto detect thread num" << std::endl;
        parsedArgs.threads=autodetect();
        std::cerr << "lemondb: info: running in " << getthreadNumber() << " threads" << std::endl;
    } else {
        std::cerr << "lemondb: info: running in " << parsedArgs.threads << " threads" << std::endl;
    }

    ThreadPool::getInstance().Initialize(parsedArgs.threads>=1? parsedArgs.threads:1);


    QueryParser p;

    p.registerQueryBuilder(std::make_unique<QueryBuilder(Debug)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(ManageTable)>());
    p.registerQueryBuilder(std::make_unique<QueryBuilder(Complex)>());

    std::stack<std::unique_ptr<std::ifstream>> fsstack;

    while (!Database::getInstance().readyExit) {
        try {
            std::string queryStr;
            if (fsstack.empty()) {
                //std::cout << "b1" << std::endl;
                queryStr = extractQueryString(is);
               // std::cout << "STR: " << queryStr << std::endl;
            } else {
                // if the listen file reach end
                try {
                    queryStr = extractQueryString(*fsstack.top());
                } catch (const std::ios_base::failure& e) {
                    fsstack.pop();
                    continue;
                }
            }
            Query::Ptr query = p.parseQuery(queryStr);
//            std::cout << query->queryName << std::endl;
//            std::cout << query->queryName.length() << std::endl;
//            if (query->queryName == "") {
//              std::cout << "???" << std::endl;
//           }
            if (query->queryName == "LISTEN") {
                // LISTEN query
                ListenQuery* ptr = dynamic_cast<ListenQuery*>(query.get());
                auto fin = std::make_unique<std::ifstream>(ptr->filename);
                if (!fin->is_open()) {
                    // cannnot open file
                    std::cerr << "Error: could not open " << ptr->filename << "\n";
                } else {
                    fsstack.push(std::move(fin));
                }
            } else if (query->queryName == "QUIT") {
              Database::getInstance().markFinishParse();
            }
            if (query->queryName == "QUIT") {
                Database::getInstance().addQuery(std::move(query));
                break;
            } else {
                Database::getInstance().addQuery(std::move(query));
            }
        }  catch (const std::ios_base::failure& e) {
            //std::cout << "bb" << std::endl;
            // End of input
            std::cerr << "EOF" << std::endl;
            break;
        } catch (const std::exception& e) {
            //std::cout << "bb2" << std::endl;
            std::cout.flush();
            std::cerr << e.what() << std::endl;
        }
    }
    Database::getInstance().markFinishParse();

    //std::cout << "j" << std::endl;
    ThreadPool::getInstance().joinThreads();
    //std::cout << "jf" << std::endl;
    return 0;
}
