#include <iostream>
#include "../memory_pool/memory_pool.h"

void test_allocate() {
  memory_pool<size_t> pool;
  size_t* x = pool.allocate();

  *x = 0xDEADBEEF;
  std::cout << std::hex << *x << std::endl;

  pool.deallocate(x);

}

void test_create_element() {
  memory_pool<size_t> pool;
  size_t* x = pool.create_element();
  
  *x = 0xADDFEDE1234;
  std::cout << std::hex << *x << std::endl;

  pool.delete_element(x);
}

int main() {
  //test_allocate(); 
  test_create_element(); 
  
  return 0;
}
