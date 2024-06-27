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
#include "GraST/Utility.h"
#include "cxxopts/cxxopts.hpp"

#include<iostream>
#include<vector>
#include<stdlib.h>
#include <filesystem>

int main(int argC, char *argV[]) {

  cxxopts::Options options("gp", "graph properties");
  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("h,help", "print usage")
    ;
  
  std::string fname;

  try{ 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    fname = result["file"].as<std::string>();
  }
  catch(cxxopts::exceptions::exception &exp) {
    std::cout<<options.help()<<std::endl;
    exit(1);
  }

  //srand(341287);
  TrueStreamIn sio(fname);  
  
  NODE_T n = sio.getNumberOfVert(); 

  WeightEdgeSim we;
  std::vector<NODE_T> degrees(n,0);
  VAL_T maxW = -1; 
  VAL_T minW = -1;

  while(sio.nextElement(we)) {
    if(we.u < we.v && (we.weight > 0)) {
      if(maxW == -1 || we.weight > maxW) maxW = we.weight;
      if(minW == -1 || we.weight < minW) minW = we.weight;
      degrees[we.u]++; 
      degrees[we.v]++; 
    }
  }
  
  NODE_T maxD = *std::max_element(degrees.begin(),degrees.end());
  NODE_T minD = *std::min_element(degrees.begin(),degrees.end());
  double avgD = std::accumulate(degrees.begin(), degrees.end(), 0.0) / degrees.size();

  std::cout<<"Graph: "<<getLastPartOfFilepath(fname)<<"\n"
    <<"n: "<<sio.getNumberOfVert()<<"\n"
    <<"m: "<<sio.getNumberOfEdg()<<"\n"
    <<"Maximum Degree: "<<maxD<<"\n"
    <<"Minimum Degree: "<<minD<<"\n"
    <<"Average Degree: "<<avgD<<"\n"
    <<"Average Degree (Comp.): "<<2.0*sio.getNumberOfEdg()/n<<"\n"
    <<"std Degree: "<<standardDeviation(degrees)<<"\n"
    <<"max Weight: "<<maxW<<"\n"
    <<"min Weight: "<<minW<<"\n";
           
  return 0;
}
