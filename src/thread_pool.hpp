#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool
{
public:
  explicit ThreadPool(size_t num_threads)
  {
    for (size_t i = 0; i < num_threads; i++)
      workers.emplace_back([this] { workerLoop(); });
  }

  ~ThreadPool()
  {
    {
      std::lock_guard<std::mutex> lock(mutex);
      stop = true;
    }
    cv.notify_all();
    for (auto &t : workers)
      t.join();
  }

  void enqueue(std::function<void()> task)
  {
    {
      std::lock_guard<std::mutex> lock(mutex);
      tasks.push(std::move(task));
    }
    cv.notify_one();
  }

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::mutex mutex;
  std::condition_variable cv;
  bool stop = false;

  void workerLoop()
  {
    while (true)
    {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return stop || !tasks.empty(); });
        if (stop && tasks.empty())
          return;
        task = std::move(tasks.front());
        tasks.pop();
      }
      task();
    }
  }
};
