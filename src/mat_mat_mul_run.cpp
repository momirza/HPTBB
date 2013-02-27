#include "mat_mat_mul.hpp"
#include "mat_mat_mul_tbb.hpp"
#include "tbb/tick_count.h"

int main(int argc, char *argv[])
{
	srand(0);
	
	int n=8;
	if(argc>1)
		n=atoi(argv[1]);
		
	local_mat_t A(n,n), B(n,n);
	local_mat_t R1(n,n), R2(n,n);
	
	A.randomise();
	// A.dump(std::cout);
	
	B.randomise();
	// B.dump(std::cout);
	
	tick_count serial_t0 = tick_count::now();
	mat_mat_mul(R1, A, B);
	tick_count serial_t1 = tick_count::now();
	std::cout << "=====Done with Serial Version=====" <<std::endl;

	tick_count parallel_t0 = tick_count::now();
	mat_mat_mul_parallel(R2, A, B);
	tick_count parallel_t1 = tick_count::now();
	std::cout << "Done with parallel" << std::endl;
	
	// R1.dump(std::cout);
	// R2.dump(std::cout);

	if (R1 == R2) 
		std::cout<<"success"<<std::endl;
	else
		std::cout<<"fail"<<std::endl;

	std::cout << "Serial version ran in " << (serial_t1 - serial_t0).seconds() << " seconds" << std::endl
           << "Parallel version ran in " <<  (parallel_t1 - parallel_t0).seconds() << " seconds" << std::endl
           << "Resulting in a speedup of " << (serial_t1 - serial_t0).seconds() / (parallel_t1 - parallel_t0).seconds() << std::endl;
	
	
	return 0;
}
