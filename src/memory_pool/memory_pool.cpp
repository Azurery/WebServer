#include "memory_pool.h"

// 以 align 个字节为单位，来进行内存对齐
// padding_ptr() 的返回值 ret 为需要补齐的字节数
template <typename Type, size_t BlockSize>
inline typename memory_pool<Type, BlockSize>::size_type
memory_pool<Type, BLockSize>::padding_ptr(data_ptr_ p, size_type align) const noexcept {
  uintptr_t val = reinterpret_cast<uintptr_t>(p);
  auto ret = (align - val) % align;
  return ret;
}

template <typename Type, size_t BlockSize>
memory_pool<Type, BlockSize>::memory_pool() noexcept {
  cur_block_ = nullptr;
  cur_slot_ = nullptr;
  last_slot_ = nullptr;
  free_slot_ = nullptr;
}

template <typename Type, size_t BlockSize>
memory_pool<Type, BlockSize>::memory_pool(const memory_pool& other) noexcept {
  
}

template <typename Type, size_t BlockSize>
memory_pool<Type, BlockSize>::memory_pool(memory_pool&& other) {
  
}

template <typename Type, size_t BlockSize>
inline typename momory_pool<Type, BlockSize>::pointer
memory_pool<Type, BlockSize>::get_address(reference ref) const noexcept {
  return &ref;
} 

// allocate_block() 为 private 方法
// 它的主要作用是：分配一块大小为 BlockSize 大小的内存区域 new_block
// 然后，需要对这块内存区域进行链接，然后进行内存补齐
template <typename Type, size_t BlockSize>
void memory_pool<Type, BlockSize>::allocate_block() {
  // 1. 利用 operator new 来分配一块大小为 BlockSize 的内存区域 new_block
  data_ptr_ new_block = reinterpret_cast<data_ptr>(operator new(BlockSize));

  // 2. 对这块内存区域进行内存对齐
  // 首先，data 的大小为：new_block 的大小 + slot_ptr 的大小
  // 接着，对 data 进行内存对齐，使得 data 的大小为slot_type 的整数倍
  data_ptr data = new_block + sizeof(slot_ptr);
  size_type aligned_data = padding_ptr(data, alignof(slot_type));


  

  // 3. 进行链接操作，形成一个内存链表
  // 个人觉得，这里的实现应该为：
  //  cur_block->next = new_block;
  //  cur_block = new_block;
  reinterpret_cast<slot_ptr>(new_block)->next = cur_block_;
  cur_block_ = reinterpret_cast<slot_ptr>(new_block);

  cur_slot_ = reinterpret_cast<slot_ptr>(data)
}
