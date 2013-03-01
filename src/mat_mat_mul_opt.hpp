#ifndef mat_mat_mul_hpp_opt
#define mat_mat_mul_hpp_opt

#include "mat_t.hpp"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include <stdio.h>
#include <assert.h>
#include <tbb/task_group.h>

using namespace tbb;		

#define CUTOFF 16

void mat_mat_mul_opt(mat_t dst, const mat_t a, const mat_t b){
    if (dst.cols <= CUTOFF || dst.rows <= CUTOFF) {
    	mat_mat_mul(dst, a, b);
    }
    else {
	    tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
	    parallel_for(blocked_range2d<unsigned>(0, dst.rows, 0, dst.cols), MatMul(&dst, &a, &b), auto_partitioner());
	}
}

#endif
