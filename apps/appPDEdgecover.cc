/*
 * Copyright (C) 2022  Ferdous, S M <ferdous.csebuet@gmail.com>
 * Author: Ferdous, S M <ferdous.csebuet@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "GraST/StreamIn.h"
#include "GraST/EdgeCover.h"
#include "cxxopts/cxxopts.hpp"
#include "GraST/MemUsage.h"

#include<iostream>
#include<vector>
#include<stdlib.h>
#include<omp.h>
#include<filesystem>

std::fstream fWrite;
void writeResults(std::string inFile, std::string alg,Stat stat) {
  fWrite<<inFile<<","<<stat.n<<","<<stat.m <<","<<alg<<","<<stat.streamReadTime
        <<","<<stat.streamProcessTime<<","<<stat.totalWeight<<","<<stat.card
        <<","<<stat.mem<<std::endl;
}

int main(int argC, char *argV[]) {

  auto baseline = getPeakRSS();
  cxxopts::Options options(argV[0], "primal dual edge cover");

  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("out,res_out","output results", cxxopts::value<std::string>()->default_value(""))  
    ("h,help", "print usage")
  ;

  std::string fname,resFile; //, method;

  try { 
		auto result = options.parse(argC,argV);
		if (result.count("help")) {
			std::cout<< options.help() << "\n"; 
			std::exit(0);
		}
		fname = result["file"].as<std::string>();
    resFile = result["res_out"].as<std::string>();
  }
	catch(cxxopts::exceptions::exception &exp) {
		std::cout<<options.help()<<std::endl;
		exit(1);
	}
  
  WeightEdgeSimList edgeList;
  Stat stat;
  NODE_T n;
  EDGE_T m;
  
  double t1 = omp_get_wtime();
  readEdgeList(fname, n, m, edgeList);
  double readTime = omp_get_wtime() - t1;
  
  double t2 = omp_get_wtime();
  std::vector<NODE_T> cV;
  WeightEdgeSimList coverEdgeList;
  primalDualEC(n,m,cV, edgeList, coverEdgeList);
  double ecTime = omp_get_wtime() - t2;
  //Match Stats
  SUM_T totWeight = 0.0;
  NODE_T card=0;

  for (auto e : coverEdgeList) {
    totWeight += e.weight;
    card++;
  }
  stat.n = n;
  stat.m = m;
  stat.totalWeight = totWeight;
  stat.card = card;
  stat.mem = getPeakRSS()-baseline;
  //used for read and process time
  stat.streamReadTime = readTime;
  stat.streamProcessTime = ecTime;

  std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<stat.totalWeight<<" "<<stat.card<<" "<<readTime<<" "<<ecTime<<std::endl;

  if(resFile.empty() == false) {
    bool fexist = false;
    if(std::filesystem::exists(resFile)) {
       fexist = true; 
    }
    fWrite.open(resFile,std::fstream::out | std::fstream::app);
    if(fWrite.is_open() == false) {
      std::cout<<"Could not open the result output file"<<std::endl; 
      return 1;
    }   
    if(fexist == false) 
      fWrite<<"graph,n,m,algorithm,read,process,weight,card,mem"<<std::endl;
    writeResults(getLastPartOfFilepath(fname),"pdthreehalf",stat);
  }
  return 0;
}
