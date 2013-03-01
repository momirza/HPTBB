#ifndef fft_tbb_hpp_seq
#define fft_tbb_hpp_seq

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


void fft_elision(
	int n, std::complex<double> wn,
	const std::complex<double> *pIn, int sIn,
	std::complex<double> *pOut, int sOut
) {
	if (n == 1){
		pOut[0] = pIn[0];
    }else if (n == 2){
		pOut[0] = pIn[0]+pIn[sIn];
		pOut[sOut] = pIn[0]-pIn[sIn];
    }else{

		unsigned m = n/2;
		task_group g;
		g.run(fftTask(m,wn*wn,pIn,2*sIn,pOut,sOut));
		g.run(fftTask(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut));
		g.wait();
		 
		std::complex<double> w=std::complex<double>(1.0, 0.0);

		for (unsigned j=0;j<m;j++){
		  std::complex<double> t1 = w*pOut[m+j];
		  std::complex<double> t2 = pOut[j]-t1;
		  pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
		  pOut[j+m] = t2;                          /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
		  w = w*wn;
		}
	}
};

	
void fft_seq(int n, const std::complex<double> *pIn, std::complex<double> *pOut)
{
	const double pi2=6.283185307179586476925286766559;
	double angle = pi2/n;
	std::complex<double> wn(cos(angle), sin(angle));
	if (n < 262144) {
		fft_impl(n, wn, pIn, 1, pOut, 1);
	}
	else { 
		fft_elision(n, wn, pIn, 1, pOut, 1);
	}
}

#endif
