#ifndef _LOG2FILE_H_
#define _LOG2FILE_H_

#include <memory>
#include <string>
#include "append_file.h"
#include "../utils/mutex.h"

//每当log2file操作执行flush_freq_次，便进行一次flush操作，会往文件中进行写操作
//只不过，文件通常也是才有缓冲区的
class log2file {
public:
  log2file(const log2file&) = delete;
  log2file operator=(const log2file&) = delete;

  log2file(const std::string& basename, int flush_freq = 1024) :
    basename_(basename), flush_freq_(flush_freq), count_(0),
    mutex_(new mutex_lock) {
      file_.reset(new append_file(basename));
  }
  ~log2file();

  void append2file(const char* log_content, int length);
  void flush();
  bool rool_file();


private:
  const std::string basename_;
  const int flush_freq_;
  int count_;
  std::unique_ptr<mutex_lock> mutex_;
  std::unique_ptr<append_file> file_;

  void append_unlocked(const char* log_content, int length) {
    file_->append2file(log_content, length);
    ++count_;
    if(count_ >= flush_freq_) {
      count_ = 0;
      file_->flush();
    }
  
  }
};

#endif
