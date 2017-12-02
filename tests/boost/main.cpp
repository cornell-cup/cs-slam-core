#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <iostream>
#include <memory>

#include "wrapper.h"

int main(int argc, char *argv[])
{
   using namespace boost::interprocess;

   int byte_value = 45;
   int memSize = 20*2;

   if(argc == 1){  //Parent process
     std::cout << "Starting Parent" << std::endl;
      //Remove shared memory on construction and destruction
      // struct shm_remove
      // {
      //    shm_remove() { shared_memory_object::remove("SharedPoints"); }
      //    ~shm_remove(){ shared_memory_object::remove("SharedPoints"); }
      // } remover;
      //
      // // shared_memory_object::remove("SharedPoints");
      //
      // //Create a shared memory object.
      // shared_memory_object shm (create_only, "SharedPoints", read_write);
      //
      // //Set size
      // shm.truncate(memSize*4);
      //
      // //Map the whole shared memory in this process
      // mapped_region region(shm, read_write);

      // int *ptr = static_cast<float*>(region.get_address());
      shared_memory_object::remove("SharedPoints");
      //Create a shared memory object.
      shared_memory_object shm(create_only, "SharedPoints", read_write);

      shm.truncate(memSize*4);

      //Set size
      mapped_region region(shm, read_write);
      Wrapper wrapper(region);
      int *ptr = wrapper.getPTR();

      ptr[0] = 1;
      std::cout << ptr << std::endl;
      std::cout << ptr[0] << std::endl;

      // std::cout << "Size: " << static_cast<int>(region.get_size()) << std::endl;
      // std::cout << "Address: " << region.get_address() << std::endl;

      for(int i = 0; i < memSize; i+=2) {
        ptr[i] = (rand() % 900) + 100;
        ptr[i+1] = rand();
        std::cout << "wrote: " << ptr[i] << std::endl;
      }

      // //Write all the memory to 1
      // std::memset(region.get_address(), byte_value, region.get_size());
      //
      // std::cout << "Region address: " << region.get_address() << std::endl;

      // while(1) {
      //   // idle
      // }

      std::cout << "Starting Child" << std::endl;
      //Launch child process
      std::string s = "./../opengl/build/build";
      if(0 != std::system(s.c_str()))
         return 1;
   }
   else{
    //  std::cout << "Starting Child" << std::endl;
    //   //Open already created shared memory object.
    //   shared_memory_object shm;
    //   try {
    //     shm = shared_memory_object(open_only, "SharedPoints", read_only);
    //   } catch (const std::exception&) {
    //     std::cout << "DNE" << std::endl;
    //     return 1;
    //   }
     //
     //
    //   //Map the whole shared memory in this process
    //   mapped_region region(shm, read_only);
     //
    //   //Check that memory was initialized to 1
    //   int *mem = static_cast<int*>(region.get_address());
    //   for(int i = 0; i < memSize; i++) {
    //      std::cout << "Read: " << *(mem++) << std::endl;
    //    }
   }
   std::cout << "Success" << std::endl;
   return 0;
}
