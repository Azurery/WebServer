#ifndef _STACK_ALLOC_H_ 
#define _STACK_ALLOC_H_

#include <memory>

template <typename Type>
struct stack_node {
  Type val;
  stack_node* prev;
};

template <typename Type, typename Alloc = std::allocator<Type>>
class stack_alloc {


};
#endif
