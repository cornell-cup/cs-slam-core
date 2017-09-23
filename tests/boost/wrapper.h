#ifndef _WRAPPER
#define _WRAPPER

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <iostream>

using namespace boost::interprocess;

class Wrapper {
public:
  Wrapper(mapped_region& region) {

    //Map the whole shared memory in this process

    _ptr = static_cast<int*>(region.get_address());
  }
  virtual ~Wrapper() {
    shared_memory_object::remove("SharedPoints");
  }
  int * getPTR() {
    return _ptr;
  }
private:
  int * _ptr;
};
#endif
