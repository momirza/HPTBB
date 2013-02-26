#include "mat_mat_mul.hpp"
#include "mat_mat_mul_tbb.hpp"

int main(int argc, char *argv[])
{
	srand(0);
	
	int n=8;
	if(argc>1)
		n=atoi(argv[1]);
		
	local_mat_t A(n,n), B(n,n);
	local_mat_t R1(n,n), R2(n,n);
	
	A.randomise();
	A.dump(std::cout);
	
	B.randomise();
	B.dump(std::cout);
	
	mat_mat_mul(R1, A, B);

	std::cout << "=====================" <<std::endl;

	mat_mat_mul_parallel(R2, A, B);
	
	R1.dump(std::cout);
	R2.dump(std::cout);

	if (R1 == R2) 
		std::cout<<"success"<<std::endl;
	else
		std::cout<<"fail"<<std::endl;
	
	
	return 0;
}
