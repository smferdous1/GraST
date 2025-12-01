#include "GraST/StreamIn.h"
#include "GraST/StreamkMatch.h"
#include "cxxopts/cxxopts.hpp"

#include<iostream>
#include<vector>
#include<stdlib.h>

int main(int argC, char *argV[]) {
  	cxxopts::Options options("stkmatch", "Streaming max weight k-matching");
  	options.add_options()
    	("f,file", "file name", cxxopts::value<std::string>())
    	("e,eps", "epsilon value", cxxopts::value<float>()->default_value("0.001"))
    	("k,kval", "k value", cxxopts::value<int>()->default_value("1"))
		("x,evict", "evict flag (default: false)", cxxopts::value<bool>()->default_value("false"))
		//("a,alt", "alt algorithm with stacks (default: false)", cxxopts::value<bool>()->default_value("false"))
    	("h,help", "print usage")
    ;
  
	std::string fname;
  	float epsilon; 
  	int k;
	bool evictFlag; //, altFlag;
  	try { 
		auto result = options.parse(argC,argV);
		if (result.count("help")) {
			std::cout<< options.help()<<"\n"; 
			std::exit(0);
		}
		fname = result["file"].as<std::string>();
		epsilon = result["eps"].as<float>();
		k = result["kval"].as<int>();
		evictFlag = result["evict"].as<bool>();
		//altFlag = result["alt"].as<bool>();
  	}
	catch(cxxopts::exceptions::exception &exp) {
		std::cout<<options.help()<<std::endl;
		exit(1);
	}

	srand(341287);
	
	StreamkMatch smatch(fname, k);
	std::vector<WeightEdgeSim> matchedEdges;
  	Stat stat;
	MatchOut matchOut = (!evictFlag) ? smatch.Solve(matchedEdges, stat, epsilon) : smatch.SolveAlt(matchedEdges, stat, epsilon);

	//Match Stats
	SUM_T totWeight = 0.0;
	NODE_T card = 0;
	for (auto e : matchOut.matchedEdges) {
		totWeight += e.weight;
		card++;
	}
	//stat.totalWeight = totWeight;
	//stat.card = card;
	std::cout << std::fixed << "Graph: " << fname << ", k = " << k << ", eps = " << epsilon << std::endl;
	std::cout << "|V| = " << stat.n << ", " << "|E| = "<< stat.m << std::endl;
	std::cout << "w(F) = " << totWeight << ", |F| = " << card << std::endl; 
	std::cout << "Stack Size: " << matchOut.stackSize << ", Stream Read Time: " << stat.streamReadTime << ", Stream Process Time: " << stat.streamProcessTime << std::endl;
	std::cout << "Post-Processing Time: " << stat.streamPostProcessTime << std::endl;

	return 0;
}
