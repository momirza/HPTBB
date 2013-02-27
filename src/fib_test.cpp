
#include "tbb/task_scheduler_init.h"
#include <tbb/task.h>
#include <iostream>
#include <stdlib.h>
#include "tbb/tick_count.h"

using namespace tbb;

#define CutOff 25



long SerialFib( long n ) {
 if( n<2 )
  return n;
 else
  return SerialFib(n-1)+SerialFib(n-2);
}

class FibTask: public task {
public:
 const long n;
 long* const sum;
 FibTask( long n_, long* sum_ ) :
     n(n_), sum(sum_) {}

 task* execute() {  
  if( n<CutOff ) {
   *sum = SerialFib(n);
  } else {
   long x, y;
   FibTask& a = *new( allocate_child() )
       FibTask(n-1,&x);
   FibTask& b = *new( allocate_child() ) 
       FibTask(n-2,&y);
   set_ref_count(3);
   spawn( b );
   spawn_and_wait_for_all( a );
   *sum = x+y;
  }
  return NULL;
 }
};


long ParallelFib( long n ) {
 long sum;
 FibTask& a = *new(task::allocate_root()) 
     FibTask(n,&sum);
 task::spawn_root_and_wait(a);
 return sum;
}


int main(int argc, char *argv[]){

  tbb::task_scheduler_init init;

  int n=8;
  if(argc>1)
    n=atoi(argv[1]);

  tick_count serial_t0 = tick_count::now();
  int sum1 = SerialFib(n);
  tick_count serial_t1 = tick_count::now();

  tick_count parallel_t0 = tick_count::now();
  int sum2 = ParallelFib(n);
  tick_count parallel_t1 = tick_count::now();

  std::cout << "Serial version ran in " << (serial_t1 - serial_t0).seconds() << " seconds" << std::endl
           << "Parallel version ran in " <<  (parallel_t1 - parallel_t0).seconds() << " seconds" << std::endl
           << "Resulting in a speedup of " << (serial_t1 - serial_t0).seconds() / (parallel_t1 - parallel_t0).seconds() << std::endl;
  
  std::cout << sum1 << std::endl << sum2 << std::endl;
}