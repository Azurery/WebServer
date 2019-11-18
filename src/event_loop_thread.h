#ifndef _EVENT_LOOP_THREAD_H_ 
#define _EVENT_LOOP_THREAD_H_

#include <functional>
#include "event_loop.h"
#include "utils/condition.h"
#include "utils/mutec.h"
#include "utils/thread.h"

class event_loop_thread {
public:
  event_loop_thread(const event_loop_thread&) = delete;
  event_loop_thread& operator=(const event_loop_thread&) = delete;

  event_loop_thread():
    loop_(NULL),
    exiting_(false),
    thread_(std::bind(&event_loop_thread::thread_function, this), "event_loop_thread"),
    mutex_(),
    cond_(mutex_) {}

  ~event_loop_thread() {
    exiting_ = true;
    if(loop_ != NULL) {
      loop_->quit();
      thread_.join();
    }
  }

  event_loop* start_loop();

private: 
    event_loop* loop_;
    bool exiting_;
    thread thread_;
    mutex_lock mutex_;
    condition cond_;

    void thread_function();
};


#endif
