#include <queue>
#include <unistd.h>
#include <sys/time.h>
#include "timer.h"

void timer_node::update(int timeout) {
  struct timeval now;
  gettimeofday(&now, NULL);
  expired_time_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;  
}

bool timer_node::is_valid() {
  struct timeval now;
  gettimeofday(&now, NULL);
  size_t temp_time = ((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000);
  if(temp_time < expired_time_) {
    return true;  
  } else {
    this->set_deleted();
    return false;
  }
}

void timer_node::clear_request() {
  http_data_ptr_.resert();
  this->set_deleted();
}

void timer_manager::add_timer(std::shared_ptr<http_data> http_data_ptr, int timeout) {
  timer_node_ptr new_timer_node(new timer_node(http_data_ptr, timeout));
  timer_node_queue.pus(new_timer_node);
  http_data_ptr->link_timer(new_timer_node);
}


/*
 * handle_expired_event() 的处理逻辑为：
 *    1. 优先队列不知随机访问
 *    2. 即使支持，随机删除某个节点后，会破坏堆的结构，需要重新更新堆结构
 *
 *  所以，对于被置为 deleted 时，会它超时或者它前面的结点都被删除时，它才会被删除
 *
 *  一个结点被置为 deleted，它最迟会在 TIMER_TIME_OUT 时间后被删除。这样做的好处为：
 *    1. 不需要遍历优先队列，省时
 *    2. 给超时时间一个容忍的时间，就是设定的超时时间是删除的下限（并不是一到超时时间就立即删除）
 *      如果监听的请求在超时后的下一次请求中又一次出现了，就不用再重新申请 request_data 节点了，这样就可以继续利用
 *      前面的 request_data，从而减少了一次 delete 和一次 new 的时间
 *
 */
void timer_manager::handle_expired_event() {
  while(!timer_node_queue.empty()) {
    timer_node_ptr timer_now = timer_node_queue.top();
    if(timer_now->is_deleted()) {
      timer_node_queue.pop();
    } else if(timer_now->is_valid() == false) {
      timer_node_queue.pop(); 
    } else 
      break; 
  }
}

