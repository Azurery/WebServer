#ifndef _CHANNEL_H_
#define _CHANNEL_H_ 

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <sys/epoll.h>
#include "timer.h"

class event_loop;
class http_data;

/*
 * channel类:channel是Reactor结构中的事件,它自始至终都只属于一个EventLoop,负责一个文件描述符的IO事件
 *            在channel类中,保存了IO事件的类型以及对应的回调函数.
 *            当IO事件发生时,最终会调用到channel类中的回调函数
 *            因此,程序中所有带有读写事件的对象都会和一个channel关联,包括loop中的eventfd,listenfd,httpda等
 */

// Reactor 最核心的是事件分发机制，即将 IO multiplexing 拿到的 IO 事件分发给各个文件描述符（fd）的事件处理函数
// 每个 channel 对象自始至终只属于一个 EventLoop，因此每个 channel 对象都只属于某一个 IO 线程。
// 每个 channel 对象自始至终只负责一个文件描述符（fd）的 IO 事件分发。但它并不拥有这个 fd，也不在析构的时候
// 关闭这个 fd。
//
// channel 对象会把不同的 IO 事件分发给不同的回调，例如 write_callback、read_callback 等。
// channel 的声明周期由其 owner class 负责管理，它一般是其他 class 的直接或间接成员。
class channel {
  using callback = std::function<void()>;
  using channel_ptr = std::shared_ptr<channel>;

public:
  channel(event_loop* loop) :
    loop_(loop), events_(0), last_events_(0), fd_(0) {}

  channel(event_loop* loop, int fd) :
    loop_(loop), fd_(fd), events_(0), last_events_(0) {}

  ~channel(){}

  int get_fd() {
    return fd_; 
  }
 
  void set_fd(int fd) {
    fd_ = fd; 
  }

  void set_holder(std::shared_ptr<http_data> holder) {
    holder_ = holder; 
  }

  std::shared_ptr<http_data> get_holder() {
    std::shared_ptr<http_data> ret(holder_.lock());
    return ret;
  }

  void set_read_handler(callback&& read_handler) {
    read_handler_ = read_handler; 
  }

  void set_write_handler(callback&& write_handler) {
    write_handler_ = write_handler; 
  }
  
  void set_error_handler(callback&& error_handler) {
    error_handler_ = error_handler; 
  }

  void set_connect_handler(callback&& conncect_handler) {
    connect_handler_ = connect_handler; 
  }

  void set_revents(uint32_t revents) {
    revents_ = revents; 
  }

  void set_events(uint32_t events) {
    events_ = events; 
  }

  uint32_t& get_events() {
    reutrn events_; 
  }

  uint32_t get_last_events() {
    return last_events_; 
  }

  bool update_last_events() {
    bool ret = (last_events_ == events_);
    last_events_ = events_;
    return ret;
  }

  void handle_read();
  void handle_wrie();
  void handle_eror(int fd, int num_error, std::string msg);
  void handle_connect();

  // 
  void handle_events() {
    events_ = 0;
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
      events_ = 0;
      return;
    }

    if(revents_ & EPOLLERR) {
      if(error_handler_) 
        error_handler_();
      events_ = 0;
      return;
    } 

    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
      handle_read(); 
    }

    if(revents_ & EPOLLOUT) {
      handle_write();  
    }

    handle_connect();
  }


private:
  int parse_uri();
  int parse_header();
  int analyze_request();

  callback read_handler_;
  callback write_handler_;
  callback error_handler_;
  callback connect_handler_;


private:
 event_loop* loop_;
 int fd_;   
 // 由于channel是Reactor结构中的event(事件),主要负责一个fd的IO事件,所以channel必须保存这些IO事件的类型以及对应的回调函数
 // 当IO事件发生时,最终会调用channel类中的回调函数
 // 因此,程序中所有带有读写事件的对象都会和一个channel关联,所以这个fd_就是带有读写事件的对象

 uint32_t events_; // events_表示感兴趣的事件，即 channel 对象关心的 IO 事件 
 uint32_t revents_;// revents_表示目前活动的事件，由 epoller 设置
 uint32_t last_events_;//

 std::weak_ptr<http_data> holder_;


};

#endif
