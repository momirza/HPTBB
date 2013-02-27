#ifndef mat_mat_mul_hpp_tbb
#define mat_mat_mul_hpp_tbb


#include "mat_t.hpp"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include <tbb/task.h>

using namespace tbb;		

class MatTask: public task {
public:
	mat_t  dst;
	const mat_t  a;
	const mat_t  b;
	// init
	MatTask(mat_t dst_, const mat_t a_, const mat_t  b_) : dst(dst_), a(a_), b(b_) {}

task* execute() { // Overrides virtual function task::execute
	if((dst.rows==1) || (dst.cols==1)){
		for(unsigned row=0;row<dst.rows;row++){
			for(unsigned col=0;col<dst.cols;col++){
				double acc=0.0;
				for(unsigned i=0;i<a.cols;i++){
					acc += a.at(row,i) * b.at(i,col);
				}
				dst.at(row,col) = acc;
			}
		}
	} 
	else {
		local_mat_t right(dst.rows, dst.cols);

		set_ref_count(9);

		for(int i = 0; i < 4; ++i)
		{
			MatTask *tsk = new( allocate_child() ) MatTask(dst.quad(i/2,i%2), a.quad(i/2,0), b.quad(0,i%2));
			task::spawn(*tsk);
		}


		for(int i = 0; i < 4; ++i)
		{
			MatTask *tsk = new(allocate_child()) MatTask(right.quad(i/2, i%2), a.quad(i/2, 1), b.quad(1, i%2));
			task::spawn(*tsk);
		}

		wait_for_all();

#if 0
		MatTask(dst.quad(0,0), a.quad(0,0), b.quad(0,0));
		MatTask(dst.quad(0,1), a.quad(0,0), b.quad(0,1));
		MatTask(dst.quad(1,0), a.quad(1,0), b.quad(0,0));
		MatTask(dst.quad(1,1), a.quad(1,0), b.quad(0,1));


		mat_mat_mul(right.quad(0,0), a.quad(0,1), b.quad(1,0));
		mat_mat_mul(right.quad(0,1), a.quad(0,1), b.quad(1,1));
		mat_mat_mul(right.quad(1,0), a.quad(1,1), b.quad(1,0));
		mat_mat_mul(right.quad(1,1), a.quad(1,1), b.quad(1,1));
#endif

		// add dst and right
		for(unsigned row=0;row<dst.rows;row++){
			for(unsigned col=0;col<dst.cols;col++){
				dst.at(row,col) += right.at(row,col);
			}
		}

		// if (dst.rows == 3 && dst.cols == 3){
		// 	std::cout<<"parallel"<<std::endl;
		// 	right.dump(std::cout);
		// }

		}
	return NULL;
	}
};

void mat_mat_mul_parallel(mat_t dst, const mat_t a, const mat_t b){
	MatTask* tsk = new(task::allocate_root()) MatTask(dst, a, b);
	task::spawn_root_and_wait(*tsk);
}

#endif
