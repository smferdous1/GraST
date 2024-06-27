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
#include "GraST/MemUsage.h"

#include<iostream>
#include<vector>
#include<stdlib.h>
#include<omp.h>

int main(int argC, char *argV[]) {

  auto baseline = getPeakRSS();
  cxxopts::Options options("grdymatchf", "greedy matching working on feature matrix");
  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("feat,feat-mat", "Running matching on ML dataset", cxxopts::value<bool>()->default_value("false"))
    ("out,res_out","output results", cxxopts::value<std::string>()->default_value(""))  
    ("h,help", "print usage")
    ;
  
  std::string fname, resFile;
  bool feat;
  try{ 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    fname = result["file"].as<std::string>();
    feat = result["feat-mat"].as<bool>();
    resFile = result["res_out"].as<std::string>();
  }
  catch(cxxopts::exceptions::exception &exp) {
    std::cout<<options.help()<<std::endl;
    exit(1);
  }
  VAL_T offset= 0;
  if(fname.find("mnist") != std::string::npos) {
    offset = 255*255*784; 
    std::cout<<offset<<"\n";
  }
  
  std::vector<WeightEdgeSim> matchEdges;
  Stat stat;
  EDGE_T m; 
  NODE_T n;
  WeightEdgeSimList edgeList;
  edgeList.clear();
  if(feat) {
    double t1 = omp_get_wtime(); 
    FeatureMatStream sio(fname,offset);
    n = sio.getNumberOfVert();

    //construct edgeList
    //

    WeightEdgeSim we;
    while(sio.nextElement(we)) {
      if(we.u < we.v && we.weight > 0) {
        edgeList.push_back(we); 
      }
    }
    double readTime = omp_get_wtime() - t1;
    stat.streamReadTime = readTime;
    m = sio.getNumberOfEdg();
  }
  else {
    double t1 = omp_get_wtime(); 
    TrueStreamIn sio(fname,offset);
    n = sio.getNumberOfVert();

    //construct edgeList
    //

    WeightEdgeSim we;
    while(sio.nextElement(we)) {
      if(we.u < we.v && we.weight > 0) {
        edgeList.push_back(we); 
      }
    }
    double readTime = omp_get_wtime() - t1;
    stat.streamReadTime = readTime;
    m = sio.getNumberOfEdg();
  }
  

  SUM_T totWeight = 0.0;
  NODE_T card=0;

  double t1 = omp_get_wtime();
  MatchOut matchOut = greedyMatching(n,m,edgeList);
  double matchTime = omp_get_wtime() - t1;
  
  stat.streamProcessTime = matchTime;
  stat.mem = getPeakRSS()-baseline;
  //Match Stats
  for(auto e:matchOut.matchedEdges) {
    //std::cout<<e.u<<" "<<e.v<<" "<<e.weight<<"\n";
    totWeight += e.weight;
    card++;
  }
  stat.n = n;
  stat.m = m;
  stat.totalWeight = totWeight;
  stat.card = card;
  std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<stat.totalWeight<<" "<<stat.card<<" "<< stat.streamReadTime<<" "<<stat.streamProcessTime<<" "<<stat.mem<<std::endl;
  return 0;
}
