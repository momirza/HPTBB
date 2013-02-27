#ifndef mat_mat_mul_hpp_tbb
#define mat_mat_mul_hpp_tbb


#include "mat_t.hpp"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include <tbb/task.h>
#include <stdio.h>
#include <assert.h>
#include <tbb/task_group.h>


using namespace tbb;		
int TOTAL=4;

class Acc {
	mat_t _dst;
	const mat_t _right;
	public:
	void operator()( const blocked_range<unsigned>& r ) const {
		mat_t right = _right;
		mat_t dst = _dst;
		for( unsigned row=r.begin(); row<r.end(); row++ ){
			for(unsigned col=0;col<dst.cols;col++){
					dst.at(row,col) += right.at(row,col);
                        }
                }
        }
        Acc(mat_t dst,mat_t right) : _dst(dst), _right(right) {}
};

class MatTask{
    public:
        mat_t  dst;
        const mat_t  a;
        const mat_t  b;
        // init
        MatTask(mat_t dst_, const mat_t a_, const mat_t  b_) : dst(dst_), a(a_), b(b_) {}

        void operator()() { 
            if(dst.rows<=1 || dst.cols<=1){
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

                task_group group;

                for(int i = 0; i < 4; i++)
                {
                    if (TOTAL>0)
                    {
                    --TOTAL;
                    group.run(MatTask(dst.quad(i/2,i%2), a.quad(i/2,0), b.quad(0,i%2)));
                    group.run(MatTask(right.quad(i/2, i%2), a.quad(i/2, 1), b.quad(1, i%2)));
                    }
                    else
                    {
                    mat_mat_mul(dst.quad(i/2,i%2), a.quad(i/2,0), b.quad(0,i%2));
                    mat_mat_mul(right.quad(i/2,i%2), a.quad(i/2,1), b.quad(1,i%2));
                    }
                }
                group.wait(); 

            parallel_for(blocked_range<unsigned>(0, dst.rows), Acc(dst, right), auto_partitioner());
            }
    }
};

void mat_mat_mul_parallel(mat_t dst, const mat_t a, const mat_t b){
    tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
    MatTask(dst, a, b)();
}

#endif
