#include "event_loop_thread.h"

void event_loop_thread::thread_function() {
  event_loop loop;
  {
    mutex_lock_guard lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }
  
  loop.loop();
  loop_ = NULL;
}


event_loop* event_loop_thread::start_loop() {
  assert(!thread_.started());
  thread_.start();
  {
    mutex_lock_guard lock(mutex_);
    //一直等到 thread_function 在 thread 里真正跑起来
    while(loop_ == NULL) {
      cond_.wait(); 
    }
  }

  return loop_;
}

