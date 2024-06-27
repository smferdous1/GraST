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
#include "GraST/StreamEC.h"
#include "cxxopts/cxxopts.hpp"
#include "GraST/MemUsage.h"

#include<iostream>
#include<vector>
#include<stdlib.h>
#include<filesystem>

std::fstream fWrite;

void writeResults(std::string inFile, std::string alg,Stat stat, float epsilon) {
  fWrite<<inFile<<","<<stat.n<<","<<stat.m <<","<<epsilon
        <<","<<alg<<","<<stat.edgeRetained<<","<<stat.streamReadTime<<","
        <<stat.streamProcessTime<<","<<stat.streamReadProcessTime<<","
        <<stat.streamPostProcessTime<<","<<stat.totalWeight<<","<<stat.card<<","<<stat.mem<<","<<stat.simStream<<std::endl;
}

void printResults(std::string inFile, std::string alg,Stat stat, float epsilon) {
  std::cout<<"\n*************************************************************"<<std::endl;
  std::cout<<"Problem Name: "<<inFile<<"\n"
            <<"#Nodes: "<<stat.n<<"\n"
            <<"#Edges: "<<stat.m<<"\n"
            <<"Epsilon: "<<epsilon<<"\n"
            <<"Name of Alg.: "<<alg<<"\n"
            <<"Is simulated?: "<<stat.simStream<<"\n"
            <<"#Edges reatined in stack: "<<stat.edgeRetained<<"\n"
            <<"Stream read time: "<<stat.streamReadTime<<"\n"
            <<"Stream read+process time: "<<stat.streamReadProcessTime<<"\n"
            <<"Stream post-process time: "<<stat.streamPostProcessTime<<"\n"
            <<"Stream total weight: "<<stat.totalWeight<<"\n"
            <<"Cardinality: "<<stat.card<<"\n"
            <<"Maximum Resident Set Size (KB): "<<stat.mem<<"\n";
  std::cout<<"*************************************************************"<<std::endl;
          
}

int main(int argC, char *argV[]) {

  auto baseline = getPeakRSS();
  cxxopts::Options options(argV[0], "Streaming Edge Cover");
  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("a,algorithm", "algo name: nn ecpd twopass", cxxopts::value<std::string>()->default_value("ecpd"))
    ("e,eps", "epsilon value for twopass", cxxopts::value<float>()->default_value("0.001"))
    ("sim,simst", "simulate stream by loading the graph in memory", cxxopts::value<bool>()->default_value("false"))
    ("out,res_out","output results", cxxopts::value<std::string>()->default_value(""))  
    ("h,help", "print usage")
    ;
  
  std::string fname,alg,resFile;
  float epsilon; 
  bool sim;
  try{ 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    fname = result["file"].as<std::string>();
    alg = result["algorithm"].as<std::string>();
    epsilon = result["eps"].as<float>();
    sim = result["simst"].as<bool>();
    resFile = result["res_out"].as<std::string>();
  }
  catch(cxxopts::exceptions::exception &exp) {
    std::cout<<options.help()<<std::endl;
    exit(1);
  }

  
  StreamEC sec(fname);
  std::vector<WeightEdgeSim> edgeCover;
  std::vector<EndPoint> edgePointers;
  Stat stat;
  SUM_T totWeight = 0.0;
  NODE_T card=0;

  if(alg == "nn" || alg == "ecpd") {
    if(alg == "nn") {
      if(sim) sec.ECNN<StreamIn>(edgePointers,stat);
      else    sec.ECNN<TrueStreamIn>(edgePointers,stat);
    }
    else if(alg == "ecpd") {
      if(sim) sec.ECPD<StreamIn>(edgePointers,stat);
      else    sec.ECPD<TrueStreamIn>(edgePointers,stat);
    }

    std::vector<bool> covered(stat.n, false);

    for (NODE_T i=0; i < stat.n; i++) {

      NODE_T u = i;
      NODE_T v = edgePointers[i].v;
      VAL_T w = edgePointers[i].weight;


      if (w > 0 && covered[u] == false) {
        //std::cout<<u<<" "<<v<<" "<<w<<std::endl;
        totWeight += w;
        card++;
        covered[u] = true;
        if (edgePointers[v].v == u)
          covered[v] = true;
      }
    }
  }
  else if(alg =="twopass") {
    if(sim) sec.TwoPass<StreamIn>(edgeCover,stat,epsilon);
    else sec.TwoPass<TrueStreamIn>(edgeCover,stat,epsilon);

    for(auto e:edgeCover) {
      //std::cout<<e.u<<" "<<e.v<<" "<<e.weight<<std::endl;
      totWeight += e.weight;
      card++;
    }
  }
  stat.totalWeight = totWeight;
  stat.card = card;
  stat.mem = getPeakRSS()-baseline;
  stat.simStream = sim;

  printResults(getLastPartOfFilepath(fname),alg,stat,epsilon);
  //std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<totWeight<<" "<<card<<" "<<stat.stackSize<<" "<<stat.streamReadProcessTime<<" "<<stat.streamPostProcessTime<<" "<<stat.mem<<std::endl;

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
      fWrite<<"graph,n,m,eps,algorithm,edge-reatined,read,process,read+process,postprocess,weight,card,mem,simulate"<<std::endl;
    writeResults(getLastPartOfFilepath(fname),alg,stat,epsilon);
  }
  return 0;
}
