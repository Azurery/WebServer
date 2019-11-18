#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "mutex.h"

class condition {
public:
  explicit condition(mutex_lock& mutex)
    : mutex_(mutex) {
      pthread_cond_init(&cond_, NULL);    
  }

  ~condition() {
    pthread_cond_destroy(&cond_);
  }

  void wait() {
    pthread_cond_wait(&cond_, mutex_.get_mutex()); 
  }

  void notify() {
    pthread_cond_signal(&cond_); 
  }

  void notify_all() {
    pthread_cond_broadcast(&cond_);
  }

  bool wait_for_seconds(int seconds) {
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += static_cast<time_t>(seconds);
    return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.get_mutex(), &abstime); 
  }

private:
  mutex_lock& mutex_;
  pthread_cond_t cond_;
};



#endif













