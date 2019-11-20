#include "memory_pool.h"

// 以 align 个字节为单位，来进行内存对齐
// padding_ptr() 的返回值 ret 为需要补齐的字节数
template <typename Type, size_t BlockSize>
inline typename memory_pool<Type, BlockSize>::size_type
memory_pool<Type, BlockSize>::padding_ptr(data_ptr p, size_type align) const noexcept {
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
memory_pool<Type, BlockSize>::memory_pool(const memory_pool& other) noexcept :
  memory_pool() {
  
}

template <typename Type, size_t BlockSize>
memory_pool<Type, BlockSize>::memory_pool(const memory_pool&& other) noexcept {
  cur_block_ = other.cur_block_;
  other.cur_block = nullptr;
  cur_slot_ = other.cur_slot_;
  last_slot_ = other.last_slot_;
  free_slot_ = other.free_slot_;
}

template <typename Type, size_t BlockSize>
memory_pool<Type, BlockSize>::~memory_pool() noexcept {
  slot_ptr cur = cur_block_;
  while(cur != nullptr) {
    slot_ptr prev = cur->next_;
    operator delete(reinterpret_cast<void*>(cur));
    cur = prev;
  }
}


template <typename Type, size_t BlockSize>
inline typename memory_pool<Type, BlockSize>::pointer
memory_pool<Type, BlockSize>::get_address(reference ref) const noexcept {
  return &ref;
} 

// allocate_block() 为 private 方法
// 它的主要作用是：分配一块大小为 BlockSize 大小的内存区域 new_block
// 然后，需要对这块内存区域进行链接，然后进行内存补齐
template <typename Type, size_t BlockSize>
void memory_pool<Type, BlockSize>::allocate_block() {
  // 1. 利用 operator new 来分配一块大小为 BlockSize 的内存区域 new_block
  data_ptr new_block = reinterpret_cast<data_ptr>(operator new(BlockSize));

  // 2. 对这块内存区域进行内存对齐
  // 首先，data 的大小为：new_block 的大小 + slot_ptr 的大小
  // 接着，对 data 进行内存对齐，使得 data 的大小为slot_type 的整数倍
  data_ptr data = new_block + sizeof(slot_ptr);
  size_type aligned_data = padding_ptr(data, alignof(slot_type)); 

  // 3. 进行链接操作，形成一个内存链表
  // 个人觉得，这里的实现应该为：
  //  cur_block->next = new_block;
  //  cur_block = new_bloc
  //  other.cur_block = nullptr;
  //  cur_slot_ = other.cur_slot_;
  //  last_slot_ = other.last_slot_;
  //  free_slot_ = other.free_slot_;k;
  reinterpret_cast<slot_ptr>(new_block)->next_ = cur_block_;
  cur_block_ = reinterpret_cast<slot_ptr>(new_block);

  cur_slot_ = reinterpret_cast<slot_ptr>(data + aligned_data);
  last_slot_ = reinterpret_cast<slot_ptr>(new_block + BlockSize - sizeof(slot_type) + 1);
}

template <typename Type, size_t BlockSize>
inline typename memory_pool<Type, BlockSize>::pointer 
memory_pool<Type, BlockSize>::allocate(size_type size, const_pointer hint) {
  if(free_slot_ != nullptr) {
    pointer ret = reinterpret_cast<pointer>(free_slot_);
    free_slot_ = free_slot_->next_;
    return ret;
  } else {
    if(cur_slot_ >= last_slot_) { 
      allocate_block();
    } 
    return reinterpret_cast<pointer>(cur_slot_++); 
  }
}
////

// 销毁指针 ptr 指向的内存区域
template <typename Type, size_t BlockSize>
inline void memory_pool<Type, BlockSize>::deallocate(pointer ptr, size_type size) {
  if(ptr != nullptr) {
    reinterpret_cast<slot_ptr>(ptr)->next_ = free_slot_;
    free_slot_ = reinterpret_cast<slot_ptr>(ptr);  
  }
}

// 在 ptr 指向的内存区域构建对象
template <typename Type, size_t BlockSize>
template <typename U, typename... Args> 
inline void memory_pool<Type, BlockSize>::construct(U* ptr, Args&&... args){
  // 利用 placement new，在已创建的内存区域上进行对象的构造
  // 即，只需要构建对象，内存区域已经指定
  new (ptr) U(std::forward<Args>(args)...);
}

template <typename Type, size_t BlockSize>
template <typename U>
inline void memory_pool<Type, BlockSize>::destory(U* ptr) {
  ptr->~U();
}

template <typename Type, size_t BlockSize>
template <typename... Args>
inline typename memory_pool<Type, BlockSize>::pointer
memory_pool<Type, BlockSize>::create_element(Args&&... args) {
  pointer ret = allocate();
  // construct<typename memory_pool<Type, BlockSize>::value_type>(ret, std::forward<Args>(args)...);
  construct<value_type>(ret, std::forward<Args>(args)...);
  return ret;
}


template <typename Type, size_t BlockSize>
inline void memory_pool<Type, BlockSize>::delete_element(pointer ptr) {
  if(ptr != nullptr) {
    ptr->~value_type(); // ?
    deallocate(ptr); 
  }
}


