#include "fft.hpp"
#include "fft_tbb.hpp"
#include "fft_opt.hpp"
#include "fft_seq.hpp"
#include <vector>
#include <stdio.h>
#include "tbb/tick_count.h"

int main(int argc, char *argv[])
{
	srand(0);
	
	if(argc<2){
		fprintf(stderr, "Specify log2 of transform size.");
		exit(1);
	}
	
	int log2n=atoi(argv[1]);
	int n=1<<log2n;
	std::vector<std::complex<double> > in(n, 0.0), out(n);
	for(int j=0;j<n;j++){
		in[j]=std::complex<double>(rand()/(double)(RAND_MAX) - 0.5, rand()/(double)(RAND_MAX) - 0.5);
	}
	
	tbb::tick_count serial_t0 = tbb::tick_count::now();
	fft(n, &in[0], &out[0]);
	tbb::tick_count serial_t1 = tbb::tick_count::now();

	std::vector<std::complex<double> >  out2(n);
	
	tbb::tick_count tbb_t0 = tbb::tick_count::now();
	fft_tbb(n, &in[0], &out2[0]);
	tbb::tick_count tbb_t1 = tbb::tick_count::now();

	std::vector<std::complex<double> >  out3(n);

	tbb::tick_count opt_t0 = tbb::tick_count::now();
	fft_opt(n, &in[0], &out3[0]);
	tbb::tick_count opt_t1 = tbb::tick_count::now();

	std::vector<std::complex<double> >  out4(n);

	tbb::tick_count seq_t0 = tbb::tick_count::now();
	fft_seq(n, &in[0], &out4[0]);
	tbb::tick_count seq_t1 = tbb::tick_count::now();

	

	// if(out2 == out && out == out3 && out == out4)  std::cout << "----- success ------" << std::endl;
	
	std::cout << (serial_t1 - serial_t0).seconds() << " " << (tbb_t1 - tbb_t0).seconds() << " " << (opt_t1 - opt_t0).seconds() << " " << (seq_t1 - seq_t0).seconds() << std::endl;
	
	
	/* To test this, you can try loading the output into matlab. Load
		the output as a four column matrix x. Then the input is:
			in=x(:,1)+x(:,2)*i
		and the output is:
			out=x(:,3)+x(:,4)*i
		You can then do fft(in) to check what matlab thinks it should be.
		Note that this fft and matlab fft produce outputs in different
		orders. How could you "sort" (hint-hing) that out in matlab so that
		you can check you have the same spectrum.
	
		You can also try putting in various types of sine wave, and checking
		whether you get the right frequency peaks in the output. You're
		the signal processing experts, I assume you know what a fourier
		transform does.
	
		Note that this not a very accurate fft, so you should expect
		the error to grow as the transform size grows.
	*/
	
	return 0;
}
