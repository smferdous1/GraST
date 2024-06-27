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
#include<omp.h>

int main(int argC, char *argV[]) {

  cxxopts::Options options(argV[0], "post processing with locally dominant matching");

  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("m,method", "LD approaches: standard, twophases, twophasespar", cxxopts::value<std::string>()->default_value("twophases"))
    ("nt,nthread", "number of threads", cxxopts::value<int>()->default_value("1"))
    ("h,help", "print usage")
    ;
  
  std::string fname,method;
  int nt;
  float epsilon; 
  try{ 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    fname = result["file"].as<std::string>();
    std::cout<<fname<<"\n";
    method = result["method"].as<std::string>();
    nt = result["nthread"].as<int>();
  }
  catch(cxxopts::exceptions::exception &exp) {
    std::cout<<options.help()<<std::endl<<std::flush;
    exit(1);
  }

  omp_set_num_threads(nt);
  NODE_T n;
  AdjacencyList adjacencyList;
  readOMtx(n,adjacencyList,fname);

  std::cout<<"read completes"<<std::endl; 
  MatchOutLite matchOut;
  if (method == "standard")
    matchOut = ldMatching(n,adjacencyList);
  else if (method == "twophases")
    matchOut = ldMatchingTwoPhases(n,adjacencyList);
  else if (method == "twophasespar")
    matchOut = ldMatchingTwoPhasesPar(n,adjacencyList);
  
  std::cout<<nt<<" "<<std::fixed<<matchOut.totalWeight<<" "<<matchOut.postProcessTime<<std::endl;
  return 0;
}
