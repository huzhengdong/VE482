#include "threadpool.h"
#include "../query/Query.h"
#include "Database.h"
#include <exception>
#include <iostream>


void ThreadPool::threadWork() {
  while (true) {
    getInstance().tasksMutex.lock();
    if (getInstance().tasks.empty()) {
      getInstance().tasksMutex.unlock();
      if (Database::getInstance().readyExit) {
        return;
      }
      std::this_thread::yield();
    } else {
      auto t = getInstance().tasks.front();
      getInstance().tasks.pop();
      getInstance().tasksMutex.unlock();
      try {
        t->execute();
      } catch (std::exception &e) {
        std::cerr << e.what() << " " << t->qptr->toString();  
      }
    }
  }  
}

void ThreadPool::Initialize(size_t threadNum) {
  if (instance == nullptr) {
    instance = std::unique_ptr<ThreadPool>(new ThreadPool(threadNum));
    for (size_t i = 0; i < threadNum; i++) {
      getInstance().threads.emplace(getInstance().threads.end(), threadWork);
    }
  }
}

ThreadPool &ThreadPool::getInstance() {
 return *instance; 
}

void ThreadPool::addTask(Task *task) {
  tasksMutex.lock();
  tasks.push(task);
  tasksMutex.unlock();
}

void ThreadPool::joinThreads() {
  for (auto &t: threads) {
    t.join();
  } 
}

std::unique_ptr<ThreadPool> ThreadPool::instance = nullptr;