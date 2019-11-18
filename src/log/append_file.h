#ifndef _APPEND_FILE_H_
#define _APPEND_FILE_H_

#include <string>

class append_file {
public:
  append_file(const append_file&) = delete;
  append_file operator=(const append_file&) = delete;

  explicit append_file(std::string file_name);
  ~append_file();

  // append2file() 会向文件中写入log信息
  void append2file(const char* log_content, const size_t length);
  void flush();
  
private:
  char buffer_[64 * 1024];
  FILE* fp_;
  size_t write(const char* log_content, size_t length) {
    return fwrite_unlocked(log_content, 1, length, fp_); 
  }

};



#endif
