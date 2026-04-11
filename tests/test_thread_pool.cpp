#include <gtest/gtest.h>
#include "thread_pool.hpp"

#include <atomic>
#include <chrono>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

TEST(ThreadPool, ExecutesSingleTask)
{
  ThreadPool pool(2);
  std::atomic<bool> ran{false};

  pool.enqueue([&] { ran = true; });

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_TRUE(ran);
}

TEST(ThreadPool, ExecutesAllEnqueuedTasks)
{
  ThreadPool pool(4);
  constexpr int N = 100;
  std::atomic<int> counter{0};

  for (int i = 0; i < N; ++i)
    pool.enqueue([&] { counter.fetch_add(1); });

  // destructor joins all threads — counter must be N after pool is destroyed
  { ThreadPool drain(0); } // no-op pool just to clarify intent
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(counter.load(), N);
}

TEST(ThreadPool, DestructorWaitsForPendingTasks)
{
  std::atomic<int> counter{0};
  {
    ThreadPool pool(2);
    for (int i = 0; i < 20; ++i)
      pool.enqueue([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        counter.fetch_add(1);
      });
  } // destructor blocks until all tasks finish
  EXPECT_EQ(counter.load(), 20);
}

TEST(ThreadPool, TasksRunOnMultipleThreads)
{
  ThreadPool pool(4);
  std::mutex mtx;
  std::set<std::thread::id> thread_ids;
  std::atomic<int> done{0};
  constexpr int N = 40;

  for (int i = 0; i < N; ++i)
    pool.enqueue([&] {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      {
        std::lock_guard<std::mutex> lock(mtx);
        thread_ids.insert(std::this_thread::get_id());
      }
      done.fetch_add(1);
    });

  { ThreadPool drain(0); }
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  EXPECT_GT(thread_ids.size(), 1u);
  EXPECT_EQ(done.load(), N);
}

TEST(ThreadPool, ZeroThreadsDestructsCleanly)
{
  // Should not hang or crash
  ThreadPool pool(0);
}

TEST(ThreadPool, EnqueueAfterHeavyLoad)
{
  ThreadPool pool(2);
  std::atomic<int> counter{0};

  for (int i = 0; i < 50; ++i)
    pool.enqueue([&] { counter.fetch_add(1); });

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_EQ(counter.load(), 50);
}
