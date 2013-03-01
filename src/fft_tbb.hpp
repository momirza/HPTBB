#ifndef fft_tbb_hpp
#define fft_tbb_hpp

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

int Log2( int n )  
{  
    // log(n)/log(2) is log2.  
    return log( n ) / log( 2 );  
}

/* Does a recursive FFT
	n = Number of elements (must be a power of two)
	wn =  Complex root of unity of order n
	(pIn,sIn) = Base and stride of input data
	(pOut,Sout) = Base and stride of output data
*/


class fftTask{
public:
	int n;
	std::complex<double> wn;
	const std::complex<double> *pIn;
	int sIn;
	std::complex<double> *pOut;
	int sOut;


	// init
	fftTask(int m_, std::complex<double> wn_, const std::complex<double> *pIn_, int sIn_, std::complex<double> *pOut_, int sOut_) :
			n(m_), wn(wn_), pIn(pIn_), sIn(sIn_), pOut(pOut_), sOut(sOut_) 
			{
			}

void operator()() { // Overrides virtual function task::execute
	// if((Log2(n) % 2) == 0){
	if(n<0){
		fft_impl(n, wn, pIn, 1, pOut, 1);
		assert(0);
	} 
	else {
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
		}
	}
};
#if 0
void fft_impl(
	int n, std::complex<double> wn,
	const std::complex<double> *pIn, int sIn,
	std::complex<double> *pOut, int sOut
){
	if (n == 1){
		pOut[0] = pIn[0];
    }else if (n == 2){
		pOut[0] = pIn[0]+pIn[sIn];
		pOut[sOut] = pIn[0]-pIn[sIn];
    }else{
		unsigned m = n/2;

		fft_impl(m,wn*wn,pIn,2*sIn,pOut,sOut);
		fft_impl(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
		 
		std::complex<double> w=std::complex<double>(1.0, 0.0);

		for (unsigned j=0;j<m;j++){
		  std::complex<double> t1 = w*pOut[m+j];
		  std::complex<double> t2 = pOut[j]-t1;
		  pOut[j] = pOut[j]+t1;                 /*  pOut[j] = pOut[j] + w^i pOut[m+j] */
		  pOut[j+m] = t2;                          /*  pOut[j] = pOut[j] - w^i pOut[m+j] */
		  w = w*wn;
		}
	}
}
#endif 

	
void fft_tbb(int n, const std::complex<double> *pIn, std::complex<double> *pOut)
{
	const double pi2=6.283185307179586476925286766559;
	double angle = pi2/n;
	std::complex<double> wn(cos(angle), sin(angle));
	// fft_impl(n, wn, pIn, 1, pOut, 1);
	//fftTask* tsk = new(task::allocate_root()) fftTask(n, wn, pIn, 1, pOut, 1);
	//task::spawn_root_and_wait(*tsk);
	fftTask(n, wn, pIn, 1, pOut, 1)();
}

#endif
