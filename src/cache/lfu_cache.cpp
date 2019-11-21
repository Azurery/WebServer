#include "lfu_cache.h"

void key_list::destory() {
  while(head_) {
    key_node prev = head_;
    head_ = head_->get_next();
    pool_.delete_element(prev);
  }
}

void key_list::add_key(key_node& node) {
  if(head_->get_next()) {
    head_->get_next()->set_prev(node);
  } else {
    tail_ = node; 
  }
  
  node->set_next(head->get_next());
  node->set_prev(head);
  head->set_next(node);
}
