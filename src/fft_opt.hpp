#ifndef fft_tbb_hpp_opt
#define fft_tbb_hpp_opt

#include <complex>
#include <math.h>
#include "mat_t.hpp"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include <tbb/task.h>
#include <tbb/mutex.h>
#include <assert.h>
#include "tbb/task_group.h"

using namespace tbb;



/* Does a recursive FFT
	n = Number of elements (must be a power of two)
	wn =  Complex root of unity of order n
	(pIn,sIn) = Base and stride of input data
	(pOut,Sout) = Base and stride of output data
*/

	
void fft_opt(int n, const std::complex<double> *pIn, std::complex<double> *pOut)
{
	const double pi2=6.283185307179586476925286766559;
	double angle = pi2/n;
	std::complex<double> wn(cos(angle), sin(angle));
	if (n < 262144) {
		fft_impl(n, wn, pIn, 1, pOut, 1);
	}
	else { 
		tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
		fftTask(n, wn, pIn, 1, pOut, 1)();
	}
}

#endif
