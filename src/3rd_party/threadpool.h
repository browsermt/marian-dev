/*

Copyright (c) 2012 Jakob Progsch, Václav Zeman

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.


This source code has been modified to have optional bounded size.
*/

#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "common/logging.h"

namespace marian {

class ThreadPool {
 public:
    explicit ThreadPool(size_t threads = 0, size_t bound /* bound on size, or 0 for unbounded */ = 0);
    void reserve(size_t threads);

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();

    size_t getNumTasks() const {
    #if USE_PTHREADS
      return tasks.size();
    #else
      ABORT("ThreadPool::getNumTasks() not supported");
    #endif
    }

    void wait_for_one(std::unique_lock<std::mutex>& lock) {
    #if USE_PTHREADS
      waiting_threads++;
      sync_condition.notify_all();
      sync_condition.wait(lock, [this]{ return continue_work; });
      waiting_threads--;
    #else
      ABORT("ThreadPool::wait_for_one not supported");
    #endif
    }

    void wait_for_others(std::unique_lock<std::mutex>& lock) {
    #if USE_PTHREADS
      continue_work = false;
      sync_condition.wait(lock, [this]{
        return waiting_threads == workers.size() - 1;
      });
    #else
      ABORT("ThreadPool::wait_for_others not supported");
    #endif
    }

    void notify_others() {
    #if USE_PTHREADS
      continue_work = true;
      sync_condition.notify_all();
    #else
      ABORT("ThreadPool::notify_others not supported");
    #endif
    }

    void join_all() {
    #if USE_PTHREADS
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
      }
      bounded_condition.notify_all();
      condition.notify_all();
      for (std::thread &worker: workers) {
        worker.join();
      }
    #else
      ABORT("ThreadPool::join_all not supported");
    #endif
    }

 private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the queue of pending tasks
    std::queue< std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::size_t bound;
    std::condition_variable bounded_condition;
    bool stop;
    std::condition_variable sync_condition;
    bool continue_work{true};
    size_t waiting_threads{0};
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads, size_t in_bound)
  : bound(in_bound), stop(false) {
  #if USE_PTHREADS
    ABORT_IF(getThrowExceptionOnAbort(), "Throwing of MarianRuntimeException not presently supported in threads");
    reserve(threads);
  #else
    bound; stop; continue_work; waiting_threads; // Make compiler warnings silent on wasm
    ABORT("ThreadPool::ThreadPool not supported");
  #endif
}

// allow callers to increase the number of threads after the fact
inline void ThreadPool::reserve(size_t threads) {
#if USE_PTHREADS
    while (workers.size() < threads)
      workers.emplace_back(
          [this] {
              for(;;) {
                  std::function<void()> task;
                  {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock,
                        [this]{ return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                  }
                  this->bounded_condition.notify_one();
                  task();
              }
          }
      );
#else
    ABORT("ThreadPool::reserve not supported");
#endif
}

// add new work item to the pool
template<class F, class... Args>
inline auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
#if USE_PTHREADS
  using return_type = typename std::result_of<F(Args...)>::type;

  auto inner_task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
  auto outer_task = [inner_task]() -> return_type {
    try {
      return inner_task();
    }
    catch(const std::exception& e) {
      ABORT("Caught std::exception in sub-thread: {}", e.what());
    }
    catch(...) {
      ABORT("Caught unknown exception in sub-thread");
    }
  };

  auto task = std::make_shared<std::packaged_task<return_type()>>(outer_task);

  std::future<return_type> res = task->get_future();
  {
      std::unique_lock<std::mutex> lock(queue_mutex);
      this->bounded_condition.wait(lock, [this] { return this->tasks.size() < this->bound || this->bound == 0 || this->stop; });
      // don't allow enqueueing after stopping the pool
      if (stop) {
        throw std::runtime_error("enqueue on stopped ThreadPool");
      }

      tasks.emplace([task](){
          (*task)();
      });
  }
  condition.notify_one();
  return res;
#else
  ABORT("ThreadPool::enqueue not supported");
#endif
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
#if USE_PTHREADS
  if(!stop)
    join_all();
#endif
}

// helper class to wait for procedural tasks (no return value) submitted into the ThreadPool
// Usage:
// {
//  TaskBarrier taskBarrier;
//  taskBarrier.push_back(threadPool.emplace_back(...)); // multiple
// } // ~TaskBarrier() will wait for all submitted tasks to complete
class TaskBarrier {
  std::vector<std::future<void>> futures;
public:
  void push_back(std::future<void>&& future) {
  #if USE_PTHREADS
    futures.emplace_back(std::move(future));
  #else
    ABORT("TaskBarrier::push_back not supported");
  #endif
  }
  ~TaskBarrier() { // destructor waits until all results are available
  #if USE_PTHREADS
    for (auto&& future : futures)
      future.wait();
  #else
    LOG(info, "TaskBarrier destructor not supported");
  #endif
  }

};

}
