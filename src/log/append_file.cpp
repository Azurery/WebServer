#include "append_file.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

append_file::append_file(std::string file_name):
  fp_(fopen(file_name.c_str(), "ae")) {
  // 用户提供缓冲区
  setbuffer(fp_, buffer_, sizeof(buffer_));
}

append_file::~append_file() {
  fclose(fp_);
}

void append_file::append2file(const char* log_content, const size_t length) {
  size_t readed_bytes = this->write(log_content, length);
  size_t remained = length - readed_bytes;
  while(remained > 0) {
    size_t every_read_bytes = this->write(log_content + readed_bytes, remained);
    if(readed_bytes == 0) {
      int err = ferror(fp_);
      if(err) {
        fprintf(stderr, "append_file::append2file() failed!\n");
      }
      break;
    }
    readed_bytes += every_read_bytes;
    remained = length - readed_bytes;
  }


  void append_file::flush() {
    fflush(fp_); 
  }
}
