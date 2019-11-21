#ifndef _TIMER_H_ 
#define _TIMER_H_

#include <deque>
#include <memory>
#include <queue>
#include <unistd.h>
#include "../utils/mutex.h"
#include "http_data.h"

class http_data;

/*
 * 关于定时器:
 *  每个 SubReactor 都持有一个定时器，用于处理超时请求和长时间不活跃的连接
 *
 *  在 muduo 中，介绍了基于时间轮的实现和基于 STL 的 set 这两种实现。
 *  在我们的实现中，直接使用了 STL 中的 priority_queue，其底层是小根堆。此外，
 *  并采用惰性删除的方式：即事件的到来并不会唤醒线程，而是每次循环的最后才进行检查。
 *  如果超时了才进行删除，因为这里对超时的要求并不会很高。如果此时线程忙，那么检查时间
 *  队列的间隔也会很短；如果不忙，也给了超时请求更长的等待时间
 *
 *
 *
 *
 *
 */
class timer_node {
public:
  timer_node(std::shared_ptr<http_data> request_data, int timeout) :
    deleted_(false), http_data_ptr_(request_data) {
    struct timeval now;
    gettimeofday(&now, NULL);
    // 以毫秒计
    expired_time_ = (((now.tv_sec % 10000) * 1000 + (now.tv_usec / 1000)) + timeout; 
  }
  
  ~timer_node() {
    if(http_data_ptr_) {
      http_data_ptr_->handle_close(); 
    }
  }

  timer_node(timer_node& other) :
    http_data_ptr_(other.http_data_ptr_) , expired_time_(0) {

  }

  void update(int timeout);
  bool is_valid();
  void clear_request();
  void set_deleted() {
    deleted_ = true; 
  }

  bool is_deleted() const {
    return deleted_; 
  }

  size_t get_expired_time() const {
    return expired_time_; 
  }

private:
  bool deleted_;
  size_t expired_time_;
  std::shared_ptr<http_data> http_data_ptr_;
};

struct timer_comparation {
  bool operator()(std::shared_ptr<timer_node>& lhs,
                  std::shared_ptr<timer_node>& rhs) const {
    return (lhs->get_expired_time() > rhs->get_expired_time()); 
  }
};
class timer_magnager {
public:
  timer_magnager() {}
  ~timer_manager() {}

  void add_timer(std::shared_ptr<http_data> http_data_ptr, int timeout);
  void handle_expired_event();

private:
  using timer_node_ptr = std::shared_ptr<timer_node>;
  
  std::priority_queue<timer_node_ptr, std::deque<timer_node_ptr>, timer_cmparation>
  timer_node_queue;
};

#endif
