#ifndef mat_mat_mul_hpp_tbb
#define mat_mat_mul_hpp_tbb

#include "mat_t.hpp"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include <stdio.h>
#include <assert.h>
#include <tbb/task_group.h>

using namespace tbb;		

class MatMul {
	mat_t *_dst;
	const mat_t * _a;
	const mat_t * _b;
	public:
    void operator()( const blocked_range2d<unsigned>& r ) const {
		mat_t *dst = _dst;
		const mat_t *a   = _a;
		const mat_t *b   = _b;
		for(unsigned row=r.rows().begin();row<r.rows().end();row++){
            for(unsigned col=r.cols().begin();col<r.cols().end();col++){
                double acc=0.0;
                for(unsigned i=0;i<a->cols;i++){
                    acc += a->at(row,i) * b->at(i,col);
                }
                dst->at(row,col) = acc;
            }
        }
    }
    MatMul(mat_t *dst, const mat_t *a, const mat_t *b) : _dst(dst), _a(a), _b(b) {}
};

void mat_mat_mul_parallel(mat_t dst, const mat_t a, const mat_t b){
    tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
    parallel_for(blocked_range2d<unsigned>(0, dst.rows, 0, dst.cols), MatMul(&dst, &a, &b), auto_partitioner());
}

#endif
