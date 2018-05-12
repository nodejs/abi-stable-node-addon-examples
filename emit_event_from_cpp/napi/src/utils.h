#ifndef __SAFE_QUEUE_H
#define __SAFE_QUEUE_H

#include <queue>
#include <mutex>

template<class T>
class ThreadSafeQueue {
private:
  std::queue<T> qu;
  std::mutex m;
public:
  bool push(T elem) {
    if (elem == nullptr) {
      return false;
    }
    std::unique_lock<std::mutex> lock(m);
    qu.push(elem);
    return true;
  }

  bool next(T& elem) {
    std::unique_lock<std::mutex> lock(m);
    if (qu.empty()) {
      return false;
    }
    elem = qu.front();
    qu.pop();
    return true;
  }

  bool empty() {
    std::unique_lock<std::mutex> lock(m);
    return qu.empty();
  }
};

template <class T>
class AutoDelete
{
  public:
    AutoDelete (T * p = 0) : ptr_(p) {}
    ~AutoDelete () throw() { delete ptr_; }
  private:
    T *ptr_;
};

#endif
