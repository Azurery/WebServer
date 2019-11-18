#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <queue>
#include "channel.h"
#include "epoll.h"
#include "event_loop.h"
#include "util.h"

void channel::handle_read() {
  if(read_handler_) {
    read_handler_(); 
  }
}


void channel::handle_write() {
  if(write_handler_) {
    write_handler_(); 
  }
}


void channel::handle_connect() {
  if(connect_handler_) {
    connect_handler_(); 
  }
}
