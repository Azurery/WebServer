#ifndef _MEMORY_POOL_H_ 
#define _MEMORY_POOL_H_

#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>
#include <functional>
#include <utility>
#include <type_traits>
//#include "../utils/mutex.h"


/*
 * memory_pool 的设计理念：
 *  简而言之，memory_pool 以 slot 为基本内存分配单元，默认大小为 4 KB
 *  其中，一个 slot 成员 data 由一块内存区域和一个slot_ptr指针构成，即：
 *        -------------------------
 *        |  new_block | slot_ptr |
 *        ------------------------
 *  而其中的 new_block 由 operaor new(BlockSize) 分配得到。由于 new_block 的类型为
 *  data_ptr（即 char*）。
 *
 *  此外，需要对 slot 进行内存对齐。在没有进行内存对齐之前，一个 slot 的大小为 
 *  new_block + sizeof(slot_ptr)，而在进行内存对齐之后，以 slot_type 大小进行内存对齐，
 *  即现在的 slot 的大小为 slot_type 的整数倍。
 *
 * 其实，说白了，就是维护了一个单链表
 *          slot                              slot
 *  -------------------------        ------------------------- 
 *  |  new_block | slot_ptr | ---->  |  new_block | slot_ptr |   
 *  ------------------------         ------------------------
 *      |
 *      |
 *     \ /
 *    data_ptr
 *
 */
template <typename Type, size_t BlockSize = 4096>
class memory_pool {
public:
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using const_pointer = const Type*;
  using const_reference = const Type&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  template <typename U> 
  struct rebind {
    using other = memory_pool<U>; 
  };

  memory_pool() noexcept;
  memory_pool(const memory_pool& other) noexcept;
  memory_pool(const memory_pool&& other) noexcept;
    //template <typename  
  
  ~memory_pool() noexcept;

  memory_pool& operator=(const memory_pool&) = delete;
  memory_pool& operator=(const memory_pool&&) = delete;

  pointer get_address(reference ref) const noexcept; 
  //const_pointer get_address(const reference ref) const noexcept;

  pointer allocate(size_type size = 1, const_pointer hint = 0);
  void deallocate(pointer ptr, size_type size = 1);

  //size_type max_size() const noexcept;

  // 调用构造函数
  template <typename U, typename... Args>
  void construct(U* ptr, Args&&... args);

  template <typename U>
  void destory(U* ptr);

  template <typename... Args>
  pointer create_element(Args&&... args);

  void delete_element(pointer ptr);


private:
  
  // 用于存储内存池中的对象值
  union slot {
    value_type elem_;
    slot* next_;
  }; 

  // 数据指针
  using data_ptr = char*;
  // 对象槽 
  using slot_type = slot;
  // 对象槽指针  
  using slot_ptr = slot*;

private:

  // 指向当前内存区域
  slot_ptr cur_block_;
  
  // 指向当前内存区域的一个对象槽
  slot_ptr cur_slot_;
 
  // 指向当前内存区域的最后一个对象槽
  slot_ptr last_slot_;

  // 指向当前内存区域中的空闲对象槽
  slot_ptr free_slot_;

private:
  size_type padding_ptr(data_ptr p, size_type align) const noexcept;
  void allocate_block();
};

#include "memory_pool.cpp"
#endif
