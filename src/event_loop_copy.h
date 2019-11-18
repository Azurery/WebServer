
#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include <functional>
#include <memory>
#include <vector>
#include <iostream>
#include "channel.h"
#include "epoller.h"
#include "utils/util.h"
#include "utils/current_thread.h"
#include "utils/thread.h"
#include "log/logging.h"

class event_loop;
__thread event_loop* loop_in_this_thread;
class event_loop {
public:
  using functor = std::function<void()>;

public:

  event_loop():
    looping_(false), poller_(new epoller()), wakeup_fd_(create_eventfd()),
    quit_(false), event_handling_(false), calling_pending_functors_(false),
    thread_id_(current_thread::tid()), wakeup_channel_ptr_(new channel(this, wakeup_fd_)) {
      if(loop_in_this_thread) {
        // 
      } else {
        loop_in_this_thread = this; 
      
      }  
    
   }



private:
  bool looping_;
  std::shared_ptr<epoller> poller_;
  int wakeup_fd_;
  bool quit_;
  bool event_handling_;
  mutable mutex_lock mutex_;
  std::vector<functor> pending_functors_;
  bool calling_pending_functors_;
  const pid_t thread_id_;
  std::shared_ptr<channel> wakeup_channel_ptr_;

};

#endif
