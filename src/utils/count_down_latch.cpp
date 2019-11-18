#include "count_down_latch.h"

count_down_latch::count_down_latch(int count)
  : mutex_(), condition_(mutex_), count_(count) {}


void count_down_latch::wait() {
  mutex_lock_guard lock(mutex_);
  while(count_ > 0) {
    condition_.wait(); 
  }
}

void count_down_latch::count_down() {
  mutex_lock_guard lock(mutex_);
  --count_;
  if(count_ == 0) {
    condition_.notify_all(); 
  }

}
