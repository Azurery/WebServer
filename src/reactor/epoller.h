#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include <sys/epoll.h>
#include "channel.h"
#include "http_data.h"
#include "timer.h"

class epoller {
public:
  epoller() :
    epollfd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
      assert(epollfd_ > 0); 
  }

  ~epoller() {}

  // epoll_add() 函数用于注册新的描述符
  void epoll_add(channel_ptr request, int timeout);

  // epoll_modify() 函数主要用于修改描述符的状态
  void epoll_modify(channel_ptr request, int timeout);

  // epoll_delete() 函数主要从epoll中删除描述符
  void epoll_delete(channel_ptr request);

  std::vector<std::shared_ptr<channel>> poll();
  std::vector<std::shared_ptr<channel>> dispatch_events(int  num_events);
  
  void add_timer(std::shared_ptr<channel> request_data, int timeout);
  int get_epollfd() {
    return epollfd_; 
  }

  void handle_expired();

private:
  static const int MAX_FDS = 100000;
  int epollfd_;
  std::vector<epoll_event> events_;
  std::shared_ptr<channel> fd2channel_[MAX_FDS]; // fd2channel_表示
  std::shared_ptr<http_data> fd2chttp_[MAX_FDS];
  timer_manager timer_manager_;
};


#endif
