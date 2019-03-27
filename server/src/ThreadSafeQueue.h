#ifndef THREADSAFEQUEUE_H_
#define THREADSAFEQUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>

namespace pl {

template<typename T>
class ThreadSafeQueue {
public:
  ThreadSafeQueue() : queue(), mutex(), cv() {}
  ~ThreadSafeQueue() {}

  void push(T t) {
    std::lock_guard<std::mutex> lk(mutex);
    queue.push(t);
    cv.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lk(mutex);
    while (queue.empty())
      cv.wait(lk);

    T ret = queue.front();
    queue.pop();
    return ret;
  }

private:
  std::queue<T> queue;
  std::mutex mutex;
  std::condition_variable cv;
};

}

#endif
