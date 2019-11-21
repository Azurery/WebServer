#ifndef _LFU_CACHE_H_ 
#define _LFU_CACHE_H_

#include <string>
#include <unordered_map>
#include "../utils/mutex.h"
#include "../memory_pool/memory_pool.h"

#define LRU_CACHE_CAPACITY 10
/*
 * LFU（Least Frequently Used ，最近最少使用算法）是一种常见的缓存算法。
 * LFU 算法的思想是：
 *    如果一个数据在最近一段时间很少被访问到，那么可以认为在将来它被访问的可能性也很小。
 *    因此，当空间满时，最小频率访问的数据最先被淘汰。
 *
 * LFU 算法的描述：
 *    设计一种缓存结构，该结构在构造时确定大小，假设大小为 K，并有两个功能：
 *    1. set(key,value)：将记录(key,value)插入该结构。当缓存满时，将访问频率最低的数据置换掉。
 *    2. get(key)：返回key对应的value值。
 *     
 * 算法实现策略：考虑到 LFU 会淘汰访问频率最小的数据，我们需要一种合适的方法按大小顺序维护数据访问的频率。LFU 算法本质上可以看做是一个 top K 问题(K = 1)，即选出频率最小的元素，因此我们很容易想到可以用二项堆来选择频率最小的元素，这样的实现比较高效。最终实现策略为小顶堆+哈希表。
 *
 *
 */

template <typename Type>
class list_node {
private:
  Type val_;
  list_node* prev_;
  list_node* next_;

public:
  Type& get_value() {
    return val_; 
  }

  list_node* set_prev(list_node* node) {
    this->prev_ = node;
  }

  list_node* get_prev() {
    return prev_; 
  }

  list_node* set_next(list_node* node) {
    this->next_ = node;
  }

  list_node* get_next() {
    return next_; 
  }

};

struct key {
  std::string key_;
  std::string val_;
};

using key_node = list_node<key>*;
//using key_node = list_node<std::string>*;

class key_list {
public:
  key_list(int freq) :
    freq_(freq), head_(pool_.create_element()), tail_(head_) {
    head_->set_next(NULL);   
  }

  int get_freq() {
    return freq_; 
  }

  void add_key(key_node& node);
  void delete_key(key_node& node);
  bool is_empty() {
    return head_ == tail_; 
  }

  key_node get_last_key() {
    return tail_; 
  }
  
  void destroy();

private:
  int freq_;
  key_node head_;
  key_node tail_;
  memory_pool<list_node<key>> pool_;
};

using freq_node = list_node<key_list>*;


class lfu_cache {
public:
  lfu_cache() :
   capacity_(LFU_CACHE_CAPACITY), 
   head_(pool_.create_element()) {
    head_->get_value()  
  }
  ~lfu_cache();

  // 通过 key 返回 value
  bool get(std::string& key, std::string& value);

  // 更新 lfu 缓存
  void set(std::string& key, std::string& value);
private:
  freq_node head_;
  int capacity_;
  mutex_lock mutex_;
  
  // key 到 key_node 的映射
  std::unordered_map<std::string, key_node> key_map;

  // key 到 freq_node 的映射
  std::unordered_map<std::string, freq_node> freq_node;

  void add_freq_node(key_node& cur_key, freq_node& cur_freq);
  void delete(freq_node& node);

  memory_pool<list_node<key_list>> pool_;
};
#endif
