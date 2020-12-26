#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

struct Task;

class ThreadPool {
  static std::unique_ptr<ThreadPool> instance; 
  size_t threadNum;
  std::vector<std::thread> threads;

  std::queue<Task *> tasks;
  std::mutex tasksMutex; 

  ThreadPool(size_t tnum): threadNum(tnum) {}
  static void threadWork();

public:
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;
  ThreadPool() = delete;
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ~ThreadPool() = default;

  // should be called only once and will initize the 
  // 1) number of threads
  // 2) a number of threads in vector
  static void Initialize(size_t threadNum);

  void print_thread() {
    std::cout << threadNum;
  };

  // get the only instance
  static ThreadPool &getInstance();

  // submit a task to the threadpool
  void addTask(Task *tasks);

  // wait until all threads finish
  void joinThreads();
};



#endif /* THREADPOOL_H */
