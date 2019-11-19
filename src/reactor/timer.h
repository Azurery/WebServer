#ifndef _TIMER_H_ 
#define _TIMER_H_

#include <deque>
#include <memory>
#include <queue>
#include <unistd.h>
#include "utils/mutex.h"
#include "http_data.h"

class http_data;

class timer_node {
public:
  timer_node(std::shared_ptr<http_data> request_data, int timeout);
  


private:
  bool deleted_;
  size_t expired_time_;
  std::shared_ptr<http_data> http_data_ptr;
};


#endif
