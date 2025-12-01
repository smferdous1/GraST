#include "GraST/StreamIn.h"
#include "GraST/StreamkDM.h"
#include "cxxopts/cxxopts.hpp"

#include<iostream>
#include<vector>
#include<stdlib.h>

int main(int argC, char *argV[]) {
  	cxxopts::Options options("stkdm", "Streaming max weight k-disjoint matching");
  	options.add_options()
    	("f,file", "file name", cxxopts::value<std::string>())
    	("e,eps", "epsilon value", cxxopts::value<float>()->default_value("0.001"))
    	("k,kval", "k value", cxxopts::value<int>()->default_value("1"))
		//("g,graphviz", "create a grpahviz dot file", cxxopts::value<bool>()->default_value("false"))
		("c,cc", "do not apply common color heuristic for coloring", cxxopts::value<bool>()->default_value("false"))
		("m,merge", "do not apply merge heuristic for trimming", cxxopts::value<bool>()->default_value("false")) //, cxxopts::value<bool>()->default_value("true"))
    	("h,help", "print usage")
    ;
  
	std::string fname;
  	float epsilon; 
  	int k;
	bool cc, merge; //graphvizFlag;
  	try { 
		auto result = options.parse(argC,argV);
		if (result.count("help")) {
			std::cout<< options.help()<<"\n"; 
			std::exit(0);
		}
		fname = result["file"].as<std::string>();
		epsilon = result["eps"].as<float>();
		k = result["kval"].as<int>();
		//graphvizFlag = result["graphviz"].as<bool>();
		cc = !result["cc"].as<bool>();
		merge = !result["merge"].as<bool>();
  	}
	catch(cxxopts::exceptions::exception &exp) {
		std::cout<<options.help()<<std::endl;
		exit(1);
	}

	srand(341287);

	if (cc)
		std::cout << "Applying Common Color Heuristic" << std::endl;
	if (merge)
		std::cout << "Appying Merge Heuristic" << std::endl;
	
	StreamkDM skdm(fname, k);
    Stat stat = skdm.bMatchingAlg(epsilon, cc, merge);
	//std::vector<std::vector<WeightEdgeSim>> kDM = skdm.getkDM();
	KMatchOut matchOut = skdm.getkDMMateArray();
	std::vector<std::pair<SUM_T, NODE_T>> colorStats = skdm.getColorStats();

	std::cout << std::fixed << "Graph: " << fname << ", k = " << k << ", eps = " << epsilon << std::endl;
	std::cout << "|V| = " << stat.n << ", " << "|E| = "<< stat.m << std::endl;
	std::cout << "w(M) = " << stat.totalWeight << ", |M| = " << stat.card << std::endl; 
	for (int i = 0; i < k; i++) {
		std::cout << "w(M_" << i+1 << ") = " << colorStats[i].first << ", |M_" << i+1 << "| = " << colorStats[i].second << std::endl;
	}
	std::cout << "Stack Size: " << stat.stackSize << ", Stream Read Time: " << stat.streamReadTime << ", Stream Process Time: " << stat.streamProcessTime;
	std::cout << ", Stream Post-Processing Time: " << stat.streamPostProcessTime << std::endl;
	std::cout << "Coloring Time: " << stat.colTime << std::endl;
	std::cout << "Merge Time: " << stat.dpTime << std::endl;

	return 0;
}
