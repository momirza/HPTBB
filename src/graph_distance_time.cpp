#include "graph_distance.hpp"
#include "graph_distance_tbb.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <vector>
#include <iostream>
#include "tbb/tick_count.h"
using namespace tbb;

int main(int argc, char *argv[])
{
	if(argc!=2){
		std::cerr<<"Must specify n.\n";
		return 1;
	}
	int n=atoi(argv[1]);
	// build graph with size n
	std::vector<node> graph=build_graph(n);
	std::vector<node> graph2(graph); // make copy
	
	// dump_graph(graph);
		
	// The run-time can vary, depending on where you start from. How should you
	// take that into account when timing it?
	// use the same start when timing
	int start=rand()%n;
	// Note that it is only graph_distance that we care about

	// sleep(2);

	tick_count serial_t0 = tick_count::now();
	std::vector<int> tmp=graph_distance(graph, start);
	tick_count serial_t1 = tick_count::now();

	// sleep(2);

	tick_count parallel_t0 = tick_count::now();
	std::vector<int> tmp2=graph_distance_tbb(graph2, start);
	tick_count parallel_t1 = tick_count::now();

	// for(int i=0;i<tmp.size();i++){
	// 	fprintf(stdout, "dist(%d->%d) = %d\n", start, i, tmp[i]);
	// }
	if (tmp2 == tmp){
		std::cout << "success" << std::endl;
		}
	else{
		std::cout << "fail" << std::endl;
	}
	std::cout << start << std::endl;
	std::cout << "Serial version ran in " << (serial_t1 - serial_t0).seconds() << " seconds" << std::endl
           << "Parallel version ran in " <<  (parallel_t1 - parallel_t0).seconds() << " seconds" << std::endl
           << "Resulting in a speedup of " << (serial_t1 - serial_t0).seconds() / (parallel_t1 - parallel_t0).seconds() << std::endl;
	
	std::cout << "NOT IMPLEMENTED :( - Please ignore" << std::endl;
	
	return 0;
}
