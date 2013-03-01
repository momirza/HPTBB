#ifndef mat_mat_mul_hpp_ser
#define mat_mat_mul_hpp_ser

#include "mat_t.hpp"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include <stdio.h>
#include <assert.h>
#include <tbb/task_group.h>

using namespace tbb;		

#define CUTOFF 16

void MatMul(mat_t *dst, const mat_t *a, const mat_t *b){
	for(unsigned row=0;row<dst->rows;row++){
			for(unsigned col=0;col<dst->cols;col++){
				double acc=0.0;
				for(unsigned i=0;i<a->cols;i++){
					acc += a->at(row,i) * b->at(i,col);
				}
				dst->at(row,col) = acc;
			}
	}
}

void mat_mat_mul_ser(mat_t dst, const mat_t a, const mat_t b){
    if (dst.cols <= CUTOFF || dst.rows <= CUTOFF) {
    	mat_mat_mul(dst, a, b);
    }
    else{
	    MatMul(&dst, &a, &b);
	}
}

#endif
