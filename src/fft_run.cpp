#include "fft.hpp"
#include "fft_tbb.hpp"
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

	// for(int j=0;j<n;j++){
	// 	fprintf(stdout, "%.16lg, %.16lg, %.16lg, %.16lg\n", real(in[j]), imag(in[j]), real(out[j]), imag(out[j]));
	// }

	int log2n2=atoi(argv[1]);
	int n2=1<<log2n2;
	
	std::vector<std::complex<double> >  out2(n2);

	
	
	tbb::tick_count parallel_t0 = tbb::tick_count::now();
	fft_tbb(n2, &in[0], &out2[0]);
	tbb::tick_count parallel_t1 = tbb::tick_count::now();
	
	// for(int j=0;j<n2;j++){
	// 	fprintf(stdout, "%.16lg, %.16lg, %.16lg, %.16lg\n", real(in[j]), imag(in[j]), real(out2[j]), imag(out2[j]));
	// }
	std::cout << out2[4]) << endl;
	std::cout << out[4]) << endl;
	

	if(out2 == out) std::cout << "----- success ------" << std::endl;
	
	std::cout << "Serial version ran in " << (serial_t1 - serial_t0).seconds() << " seconds" << std::endl
           << "Parallel version ran in " <<  (parallel_t1 - parallel_t0).seconds() << " seconds" << std::endl
           << "Resulting in a speedup of " << (serial_t1 - serial_t0).seconds() / (parallel_t1 - parallel_t0).seconds() << std::endl;
	
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
