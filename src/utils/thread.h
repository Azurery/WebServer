#ifndef _THREAD_H_
#define  _THREAD_H_

#include <string>
#include <functional>
#include <memory>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "count_down_latch.h"

class thread {
  using thread_function = std::function<void()>;

public:
  thread(const thread&) = delete;
  thread& operator=(const thread&) = delete;

  explicit thread(const thread_function& thread_function_, const std::string& name = std::string()) :
    started_(false), joined_(false), pthread_id_(0), tid_(0),
    thread_function_(thread_function), name_(name), latch_(1) {
      set_default_name(); 
  }

  ~thread() {
    if(started_ && !joined_) {
      pthread_detach(pthread_id_); 
    } 
  }

  void start() {
    assert(!started_);
    started_ = true;
    thread_data* data = new thread_data(thread_function_, name_, &tid_, &latch_);
    if(pthread_create(&pthread_id_, NULL, &start_thread, data)) {
      started_ = false;
      delete data; 
    } else {
      latch_.wait();
      assert(tid_ > 0);
    }
  }

  void join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthread_id_, NULL); 
  }

  bool is_started() const {
    return started_; 
  }

  pid tid() const {
    return tid_; 
  }

  const std::string& get_name() const {
    return name_; 
  }

private:
  bool started_;
  bool joined_;
 
  pthread_t ptrhead_id_;
  pid_t tid_;
  thread_function thread_function_;
  std::string name_;
  count_down_latch latch_;

  void set_default_name() {
    if(name_.empty()) {
      buffer[32];
      snprintf(buffer, sizeof(buffer), "thread");
      name = buffer;
    }
  }
};

#endif

