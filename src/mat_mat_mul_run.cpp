#include "mat_mat_mul.hpp"
#include "mat_mat_mul_tbb.hpp"
#include "mat_mat_mul_opt.hpp"
#include "mat_mat_mul_seq.hpp"
#include "tbb/tick_count.h"

int main(int argc, char *argv[])
{
	srand(0);
	
	int n=8;
	if(argc>1)
		n=atoi(argv[1]);
		
	local_mat_t A(n,n), B(n,n);
	local_mat_t R1(n,n), R2(n,n), R3(n,n), R4(n,n);
	
	A.randomise();
	// A.dump(std::cout);
	
	B.randomise();
	// B.dump(std::cout);
	
	tick_count serial_t0 = tick_count::now();
	mat_mat_mul(R1, A, B);
	tick_count serial_t1 = tick_count::now();
	// std::cout << "=====Done with Serial Version=====" <<std::endl;

	tick_count tbb_t0 = tick_count::now();
	mat_mat_mul_tbb(R2, A, B);
	tick_count tbb_t1 = tick_count::now();

	tick_count opt_t0 = tick_count::now();
	mat_mat_mul_opt(R3, A, B);
	tick_count opt_t1 = tick_count::now();
	

	tick_count serial_elision_t0 = tick_count::now();
	mat_mat_mul_seq(R4, A, B);
	tick_count serial_elision_t1 = tick_count::now();
	// std::cout << "=====Done with elision Version=====" << std::endl;
	
	// R1.dump(std::cout);
	// R2.dump(std::cout);

	// if (R1 == R2 && R1 == R3 && R1 == R4) 
	// 	std::cout<<"****success****"<<std::endl;
	// else
	// 	std::cout<<"fail"<<std::endl;

	// std::cout << "Serial version ran in " << (serial_t1 - serial_t0).seconds() << " seconds" << std::endl
 //           << "Parallel version ran in " <<  (parallel_t1 - parallel_t0).seconds() << " seconds" << std::endl
 //           << "Resulting in a speedup of " << (serial_t1 - serial_t0).seconds() / (parallel_t1 - parallel_t0).seconds() << std::endl;
	
	std::cout << (serial_t1 - serial_t0).seconds() << " " << (tbb_t1 - tbb_t0).seconds() << " " << (opt_t1 - opt_t0).seconds() << " " << (serial_elision_t1 - serial_elision_t0).seconds() << std::endl;
	return 0;
}
