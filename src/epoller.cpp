#include <assert.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <deque>
#include <queue>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include "epoller.h"
#include "util.h"
#include "log/logging.h"

const int EVENT_SUM = 4096;
const int EPOLLWAIT_TIME = 10000;
//using channel_ptr = std::shared_ptr<channel>;


  // epoll_add() 函数用于注册新的描述符
void epoller::epoll_add(channel_ptr request, int timeout) {
    // 首先获得文件描述符
    int fd = request->get_fd();
    if(timeout > 0) {
      add_timer(request, timeout);
      fd2http_[fd] = request->get_holder();
    }
    /*
     * 结构体epoll_event的作用:1.注册感兴趣的事件 2.回传所发生待处理的事件
     *
     * typedef union epoll_data {
     *    void* ptr;
     *    __uint32_t u32;
     *    __uint32_t u64;
     * } epoll_data_t;
     *
     * struct epoll_event {
     *    __uint32_t events; // epoll event
     *    epoll_data_t data; // user data variable
     * };
     *
     * 其中,events表示:感兴趣的事件和被触发的事件
     *    EPOLLIN: 表示对应的fd可以读
     *    EPOLLOUT: 表示对应的fd可以写
     *    EPOLLPRI: 表示对应的fd有紧急的数据可读
     *    EPOLLERR: 表示对应的发生错误
     *    EPOLLHUP: 表示对应的fd被挂断
     *    EPOLLET: ET的epoll工作模式
     *
     * 所涉及到的函数:
     * 1. int epoll_create(int size);
     *    功能: 该函数生成一个epoll专用的fd
     *          其中的参数用于制定生成的fd的最大范围
     *
     * 2. int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
     *    功能: 用于控制某个fd上的事件,可以注册事件,修改事件,删除事件
     *
     *    @param epfd: 由epoll_create生成的epoll专用的fd
     *    @param op: 要进行的操作,
     *        EPOLL_CTL_ADD 注册
     *        EPOLL_CTL_MOD 修改
     *        EPOLL_CTL_DEL 删除
     *    @param fd: 关联的fd
     *    @param event: 指向epoll_event的指针
     *
     * 3. int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
     *    功能: 该函数用于轮询I/O事件的发生
     *
     *    @param epfd: 由epoll_create生成的epoll专用的fd
     *    @param events: 用于回传待处理事件的而数组
     *    @param maxevents: 每次能处理的事件数
     *    @param timeout: 等待I/O事件发生的超时值
    */
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->get_events();

    request->update_last_events();

    fd2channel_[fd] = request;
    if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0) {
      perror("epoll_add error!");
      fd2channel_[fd].reset();
    }
}

void epoller::epoll_modify(channel_ptr request, int timeout) {
  if(timeout > 0) {
    add_timer(request, timeout);
    int fd = request->get_fd();
    if(!request->update_last_events()) {
      struct epoll_event event;
      event.data.fd = fd;
      event.events = request->get_events();
      if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0) {
        perror("epoll_modify error!");
        fd2channel_[fd].reset();
      }
    }
  }
}

void epoller::epoll_delete(channel_ptr request) {
  int fd = request->get_fd();
  struct epoll_event event;
  event.data.fd = fd;
  event.events = request->get_last_events();

  if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event) < 0) {
    perror("epoll_delete error!");
  }
  fd2channel_[fd].reset();
  fd2http_[fd].reset();
}

//  分发处理函数
std::vector<std::shared_ptr<channel>> epoller::dispatch_events(int  num_events) {
  std::vector<channel_ptr> request_data;
  for(int i = 0; i < num_events; ++i) {
    // 获取有事件产生的fd
    int fd = events_[i].data.fd;
    channel_ptr cur_request = fd2channel_[fd];
  
  }

}

// poll() 函数用于返回活跃事件数
std::vector<std::shared_ptr<channel>> epoller::poll() {
  while(true) {
    int num_event = epoll_wait(epollfd_, &(*events_.begin()), events_.size(), EPOLLWAIT_TIME);
    if(num_event < 0) {
      perror("epoll wait error!"); 
    }
    std::vector<channel_ptr> request_data = 
  }


}
