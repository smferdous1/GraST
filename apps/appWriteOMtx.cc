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
#include "GraST/StreamMatch.h"
#include "cxxopts/cxxopts.hpp"

#include<iostream>
#include<vector>
#include<stdlib.h>

int main(int argC, char *argV[]) {

  cxxopts::Options options(argV[0], "Generate and store adjlist for the post processing");

  options.add_options()
    ("i,infile", "input file name", cxxopts::value<std::string>())
    ("o,outfile", "out file name", cxxopts::value<std::string>())
    ("e,eps", "epsilon value", cxxopts::value<float>()->default_value("0.001"))
    ("h,help", "print usage")
    ;
  
  std::string ifname,ofname;
  float epsilon; 
  try{ 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    ifname = result["infile"].as<std::string>();
    std::cout<<ifname<<"\n";
    ofname = result["outfile"].as<std::string>();
    std::cout<<ofname<<"\n";
    epsilon = result["eps"].as<float>();
  }
  catch(cxxopts::exceptions::exception &exp) {
    std::cout<<options.help()<<std::endl<<std::flush;
    exit(1);
  }

  srand(341287);
  

  StreamMatch smatch(ifname);
  NODE_T n;
  AdjacencyList adjList;
  smatch.GenerateAdjList(n,adjList,epsilon);
  
  writeOMtx(n,adjList,ofname);
  
  /*
  //Match Stats
  SUM_T totWeight {0.0};
  NODE_T card {0};
  for(auto e:matchOut.matchedEdges) {
    //std::cout<<e.u<<" "<<e.v<<" "<<e.weight<<"\n";
    totWeight += e.weight;
    card++;
  }
  */
  //std::cout<<std::fixed<<matchOut.n<<" "<<matchOut.m<<" "<<matchOut.totalWeight<<" "<<matchOut.card<<" "<<matchOut.stackSize<<" "<<matchOut.postProcessTime<<std::endl;
  return 0;
}
