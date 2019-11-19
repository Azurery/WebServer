#ifndef _MEMORY_POOL_H_ 
#define _MEMORY_POOL_H_

#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>
#include <functional>
#include "../utils/mutex.h"

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
  memory_pool& operator=(const memory_pool&&) = delete;i

  pointer get_address(reference ref) const noexcept;
  const_pointer get_address(const reference ref) const noexcept;

  pointer allocate(size_type size = 1, const_pointer hint = 0);
  void deallocate(pointer ptr, size_type size = 1);

  size_type max_size() const noexcept;

  template <class



private:
  int slot_size;

  

};

#endif
