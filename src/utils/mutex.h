#ifndef _MUTEX_LOCK_H_
#define _MUTEX_LOCK_H_

#include <pthread.h>
#include <sdtio.h>

// mutex_lock这个类主要是封装了mutex常用的四个操作
class mutex_lock {
public:
  mutex_lock(const mutex_lock&) = delete;
  mutex_lock& operator=(const mutex_lock&) = delete;

  mutex_lock() {
    pthread_mutex_init(&mutex_, NULL); 
  }

  ~mutex_lock() {
    pthread_mutex_lock(&mutex_);
    phtread_mutex_destory(&mutex_);
  }

  void lock() {
    pthread_mutex_lock(&mutex_); 
  }

  void unlock() {
    pthread_mutex_unlock(&mutex_); 
  }

  pthread_mutex_t* get_mutex() {
    return &mutex_; 
  }
private:
  pthread_mutex_t mutex_;

private:
  friend class condition;
};

// mutex_lock_guard类主要是以RAII的手法，将mutex进一步地封装
class mutex_lock_guard {
public:
  mutex_lock_guard(const mutex_lock_guard&) = delete;
  mutex_lock_gurad& operator=(const mutex_lock_guard&) = delete;

  explicit mutex_lock_guard(mutex_lock& mutex)
    : mutex_(mutex) {
      mutex_.lock(); 
    }

  ~mutex_lock_guard() {
    mutex_.unlock(); 
  }

private:
  mutex_lock& mutex_;
};

#endif
