#ifndef graph_distance_hpp_tbb
#define graph_distance_hpp_tbb

#include <vector>
#include <deque>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "tbb/concurrent_queue.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/concurrent_vector.h"

#include <cmath>

using namespace tbb;

#if 0
struct node
{
	int id;
	std::vector<int> edges;
};

// This is just a simple platform independent rng, returns an integer in [0,maxn]
unsigned rng(unsigned &seed, unsigned maxn)
{
	seed=(seed*1664525UL*+1013904223UL)&0xFFFFFFFF;
	double u=ldexp((double)seed, -32);	// Convert to [0,1)
	return (unsigned)floor(u*(maxn+1));
}

std::vector<node> build_graph(int n)
{
	std::vector<node> res(n);
	for(int i=0;i<n;i++){
		res[i].id=i;
		res[i].edges.reserve(15);
	}
	
	unsigned seed=1;
	
	for(int i=0;i<10*n;i++){
		int a=rng(seed,n-1);
		int b=rng(seed,n-1);
		
		res[a].edges.push_back(b);
	}
	return res;
}

void dump_graph(const std::vector<node> &nodes)
{
	for(unsigned i=0;i<nodes.size();i++){
		fprintf(stdout, "%d -> ", i);
		for(unsigned j=0;j<nodes[i].edges.size();j++){
			fprintf(stdout, " %d", nodes[i].edges[j]);
		}
		fprintf(stdout, "\n");
	}
}
#endif

class PushFor {
	concurrent_queue<std::pair<int,int> >  * _todo;
	std::vector<node> _nodes;
	std::pair<int,int> *_curr;
	public:
    void operator()( const blocked_range<int>& r ) const {
		concurrent_queue<std::pair<int,int> >  todo = *_todo;
		std::vector<node> nodes = _nodes;
		std::pair<int,int> curr = *_curr;
		for(int i=r.begin();i<r.end();i++){
				todo.push(std::make_pair(nodes[curr.first].edges[i], curr.second+1));
	    }
	}
    PushFor(concurrent_queue<std::pair<int,int> >  *todo, std::vector<node> nodes, std::pair<int,int>* curr) : _todo(todo), _nodes(nodes), _curr(curr) {}
};




/*! This is the function we are interested in the execution time of. */
// Here n is the number of nodes (nodes.size())
std::vector<int> graph_distance_tbb(const std::vector<node> &nodes, int start)
{
	std::vector<int> distance(nodes.size(), INT_MAX); // all nodes start at infinite distance
	
	// a list of (id,dist)
	concurrent_queue<std::pair<int,int> > todo; // deque is a mutable double ended queue 
	todo.push(std::make_pair(start,0));
	std::pair<int,int> curr;
	while(todo.try_pop(curr)){ // loop until todo is empty
		if(distance[curr.first]==INT_MAX){
			distance[curr.first]=curr.second;
			parallel_for(blocked_range<int>(0, nodes[curr.first].edges.size()), PushFor(&todo, nodes, &curr));
			// for(int i=0;i<nodes[curr.first].edges.size();i++){
			// 	todo.push(std::make_pair(nodes[curr.first].edges[i],curr.second+1));
			// }	
		}
	}
	
	return distance;
}

#endif
