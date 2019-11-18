#ifndef _COUNT_DOWN_LATCH_H_
#define _COUNT_DOWN_LATCH_H_

#include "condition.h"
#include "mutex.h"

// count_down_latch类的主要作用是：：完全！
// 
class count_down_latch {
public:
  count_down_latch(const count_down_latch&) = delete;
  count_down_latch operator=(const count_down_latch&) = delete;

  explicit count_down_latch(int count);
  void wait();
  void count_down();


private:
  mutable mutex_lock mutex_;
  condtion condition_;
  int count_;
};


#endif
