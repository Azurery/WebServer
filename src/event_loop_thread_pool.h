#ifndef _EVENT_LOOP_THREAD_POOL_H_ 
#define _EVENT_LOOP_THREAD_POOL_H_ 

#include <memory>
#include <vector>
#include "event_loop_thread.h"
#include "utils/logging.h"

class event_loop_thread_pool {
public:
  event_loop_thread_pool(event_loop* base_loop, int num_threads) :
    

private:
  event_loop* base_loop_;
  bool started_;
  int num_threads_;
  int next_;
  std::vector<std::shared_ptr<event_loop_thread>> threads_;
  std::vector<event_loop*> loops_;
};

#endif
