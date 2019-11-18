#ifndef _EVENT_LOOP_
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

/*
 *  event_loop类：由于本项目采用的是one loop per thread的思想，意味着：每个线程只能有一个event_loop对象
 *                event loop，即事件循环，每次从poller中取出活跃事件，并给到channel里面进行分发处理
 *               event_loop中的 loop() 函数会在最底层（thread）中被真正地调用，开始无限的循环
 *               直到某一轮检查到退出状态后，从底层一层一层地退出
 * 
 *
 *
 */

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
      
      wakeup_channel_ptr_->set_events(EPOLLIN | EPOLLET);
      wakeup_channel_ptr_->set_read_handler(std::bind(&event_loop::handle_read, this));
      wakeup_channel_ptr_->set_connect_handler(std::bind(&event_loop::handle_connect, this));
      poller_->epoll_add(wakeup_channel_ptr_, 0);
   }

  ~event_loop() {
    close(wakeup_fd_);
    loop_in_this_thread = NULL; 
  }

  void loop();
  void quit();
  void run_in_loop();
  void queue_in_loop(functor&& cb);
  void is_in_loop_thread(functor&& cb) {
    return thread_id_ == current_thread::tid();    
  } 

  void assert_in_loop_thread() const {
    assert(is_in_loop_thread()); 
  }

  void shutdown(std::shared_ptr<channel> channel) {
    shutdown_wr(channel->get_fd());
  }

  void remove_from_poller(std::shared_ptr<channel> channel) {
    poller_->epoll_delete(channel); 
  }

  void update_poller(std::shared_ptr<channel> channel, int timeout) {
    poller_->epoll_modify(channel, timeout);
  }

  void add2poller(std::shared_ptr<channel> channel, int timeout) {
    poller_->epoll_add(channel, timeout); 
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


  void wakeup();
  void handle_read();
  void do_pending_functors();
  void handle_connect();
};
#endif
