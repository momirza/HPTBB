#ifndef mat_mat_mul_hpp_tbb
#define mat_mat_mul_hpp_tbb


#include "mat_t.hpp"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include <tbb/task.h>
#include <stdio.h>

using namespace tbb;		
int TOTAL_THREADS=1;
class Acc {
	mat_t _dst;
	const mat_t _right;
	public:
	void operator()( const blocked_range2d<unsigned>& r ) const {
		mat_t right = _right;
		mat_t dst = _dst;
		for( unsigned row=r.rows().begin(); row<r.rows().end(); row++ ){
			for(unsigned col=r.cols().begin();col<r.cols().end();col++){
					dst.at(row,col) += right.at(row,col);
                        }
                }
        }
        Acc(mat_t dst,mat_t right) : _dst(dst), _right(right) {}
};

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

                if (TOTAL_THREADS <= 0){
                    printf("NO THREAD!\n");
                    mat_mat_mul(dst.quad(0,0), a.quad(0,0), b.quad(0,0));
                    mat_mat_mul(dst.quad(0,1), a.quad(0,0), b.quad(0,1));
                    mat_mat_mul(dst.quad(1,0), a.quad(1,0), b.quad(0,0));
                    mat_mat_mul(dst.quad(1,1), a.quad(1,0), b.quad(0,1));

                    mat_mat_mul(right.quad(0,0), a.quad(0,1), b.quad(1,0));
                    mat_mat_mul(right.quad(0,1), a.quad(0,1), b.quad(1,1));
                    mat_mat_mul(right.quad(1,0), a.quad(1,1), b.quad(1,0));
                    mat_mat_mul(right.quad(1,1), a.quad(1,1), b.quad(1,1));
                    printf("NO THREAD END!\n");
                }
                else{	
                    printf("THREAD!\n");
                    TOTAL_THREADS--;
                    set_ref_count(9);

                    for(int i = 0; i < 4; i++)
                    {
                        MatTask *tsk = new( allocate_child() ) MatTask(dst.quad(i/2,i%2), a.quad(i/2,0), b.quad(0,i%2));
                        task::spawn(*tsk);
                        MatTask *tsk2 = new(allocate_child()) MatTask(right.quad(i/2, i%2), a.quad(i/2, 1), b.quad(1, i%2));
                        task::spawn(*tsk2);
                    }
                    wait_for_all(); 
                }

            // paralllel add dst and right
            printf("accumulator time!\n");
            parallel_for(blocked_range2d<unsigned, unsigned>(0, dst.rows, 0, dst.cols), Acc(dst, right), auto_partitioner());
            printf("end accumulator time!\n");

            }
        return NULL;
    }
};

void mat_mat_mul_parallel(mat_t dst, const mat_t a, const mat_t b){
    tbb::task_scheduler_init init;
    MatTask* tsk = new(task::allocate_root()) MatTask(dst, a, b);
    task::spawn_root_and_wait(*tsk);
}

#endif
