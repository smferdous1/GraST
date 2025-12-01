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
#include "GraST/StreamkDM.h"
#include "GraST/StreamMatch.h"
#include "GraST/MemUsage.h"
#include "cxxopts/cxxopts.hpp"

#include<iostream>
#include<vector>
#include<stdlib.h>
#include <filesystem>

struct Flags {
  bool dp=false;
  bool maximal = false;
  bool cc = false;
  bool balance = false;
  bool merge = false;
};

std::fstream fWrite;
void writeResults( std::string, NODE_T, std::string, Stat, float epsilon);
std::string getShortAlgName(std::string, Flags);


void writeResults(std::string inFile,NODE_T k, std::string alg,Stat stat, float epsilon) {
  fWrite<<inFile<<","<<stat.n<<","<<stat.m <<","<<k<<","<<epsilon
        <<","<<alg<<","<<stat.edgeRetained<<","<<stat.streamReadTime<<","<<stat.streamProcessTime<<","
        <<stat.streamReadProcessTime<<","<<stat.streamPostProcessTime<<","<<stat.colTime<<","<<stat.dpTime
        <<","<<stat.totalWeight<<","<<stat.card<<","<<stat.mem<<std::endl;
}


void printResults(std::string inFile, std::string alg,Stat stat, float epsilon, Flags allFlags) {
  std::cout<<"\n*************************************************************"<<std::endl;
  if (stat.simStream == 1)
    std::cout<<"Simulated Stream setting. Use this setting to report read and process time. The reported read+process time is actually the process time. The memory reported does not reflect actual memory. Don't use \"--sim\" for reporting memory."<<std::endl;

  if (stat.simStream == 0)
    std::cout<<"True Stream setting, where each edge is read one by one from the file. The memory reported is the maximum resident set size. You can ignore the read time, which only accounts for initialize the stream class not actually read from file."<<std::endl;


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
            <<"Total weight: "<<stat.totalWeight<<"\n"
            <<"Total Cardinality: "<<stat.card<<"\n"
            <<"Maximum Resident Set Size (KB): "<<stat.mem<<"\n";
  std::cout<<"*************************************************************"<<std::endl;
          
}
std::string getShortAlgName(std::string algName, Flags allFlags) {

  if(algName == "stk"){
    if(allFlags.dp) algName.append("-dp");
    if(allFlags.maximal) algName.append("-max");
    if(allFlags.balance) algName.append("-bal");
  }
  else if (algName =="stkb"){
    if(allFlags.dp) algName.append("-dp");
    if(allFlags.cc) algName.append("-cc");
    if(allFlags.merge) algName.append("-m");
  }
  return algName;
}

int main(int argC, char *argV[]) {

  auto baseline = getPeakRSS();

  cxxopts::Options options("kstmatch", "Streaming max weight k-disjoint matching");
  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("a,algorithm", "algo name: stk stkb", cxxopts::value<std::string>())
    ("k,kval", "number of disjoint matchings", cxxopts::value<NODE_T>()->default_value("1"))
    ("e,eps", "epsilon value", cxxopts::value<float>()->default_value("0.001"))
    ("m,maximal", "extend to maximal matching")
    ("sim,simst", "simulate stream (only for stk) by loading the graph in memory", cxxopts::value<bool>()->default_value("false"))
    ("dp,dynamic", "apply dynamic programming on 2k disjoint matching")
    ("c,cc", "do not apply common color heuristic when coloring for stkb", cxxopts::value<bool>()->default_value("false"))
    ("n,merge", "do not apply merging heuristic when trimming for sktb", cxxopts::value<bool>()->default_value("false"))
    ("r,runs", "number of runs", cxxopts::value<int>()->default_value("1"))
    ("out,res_out","output results", cxxopts::value<std::string>()->default_value(""))
    ("h,help", "print usage")
    ;
  
  std::string fname,resFile,alg;
  float epsilon; 
  NODE_T k;
  Flags allFlags;
  int runs;
  bool sim;
  try{ 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    fname = result["file"].as<std::string>();
    alg = result["algorithm"].as<std::string>();
    resFile = result["res_out"].as<std::string>();
    epsilon = result["eps"].as<float>();
    k = result["kval"].as<NODE_T>();
    runs = result["runs"].as<int>();
    sim = result["simst"].as<bool>();
    allFlags.maximal = result["maximal"].as<bool>();
    allFlags.dp = result["dynamic"].as<bool>();
    allFlags.cc = !result["cc"].as<bool>();
    allFlags.merge = !result["merge"].as<bool>();
  }
  catch(cxxopts::exceptions::exception &exp) {
    std::cout<<options.help()<<std::endl;
    exit(1);
  }

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
      fWrite<<"graph,n,m,b,eps,algorithm,edge-reatined,read,process,read+process,post-process,color,dp,weight,card,mem"<<std::endl;
  }
  
  //srand(341287);
  
  for(int r=0;r<runs;r++) {

    KMatchOut matchOut;
    std::string algName = getShortAlgName(alg,allFlags);
    Stat stat;
    if(alg == "stk") {
      StreamMatch smatch(fname);
      if(allFlags.dp) {
        if(sim) matchOut = smatch.kDMatchGW<StreamIn>(stat,2*k,allFlags.maximal,epsilon);
        else matchOut = smatch.kDMatchGW<TrueStreamIn>(stat,2*k,allFlags.maximal,epsilon);
      }
      else{
        if(sim) matchOut = smatch.kDMatchGW<StreamIn>(stat,k,allFlags.maximal,epsilon);
        else matchOut = smatch.kDMatchGW<TrueStreamIn>(stat,k,allFlags.maximal,epsilon);
      }
    }
    else if(alg == "stkb" || alg =="stkb-f" || alg =="stkb-new") {
      StreamkDM skdm(fname, k);
      if (allFlags.dp) {
        skdm.setKVal(2*k); 
      }
      stat = skdm.bMatchingAlg(epsilon, allFlags.cc, allFlags.merge);
      //std::vector<std::vector<WeightEdgeSim>> kDM = skdm.getkDM();
      matchOut = skdm.getkDMMateArray();
    }

    //Match Stats
    SUM_T totWeight = 0.0, cumWeight = 0.0;
    NODE_T card = 0;

    /*for (NODE_T i = 0; i < k; i++) {
      totWeight = 0.0;
      for (NODE_T j = 0; j<stat.n; j++) {
        if (matchOut[i][j].v != -1) {
          totWeight += static_cast<SUM_T> (matchOut[i][j].weight);       
          card++;
        } 
      } 
      std::cout<<std::fixed<<i<<" "<<totWeight/2<<" "<<card/2<<std::endl;
      cumWeight += totWeight;
    }

    cumWeight = cumWeight/2;*/
    
    /*for(auto e:matchOut.matchedEdges) {
      //std::cout<<e.u<<" "<<e.v<<" "<<e.weight<<"\n";
      totWeight += static_cast<SUM_T> (e.weight);
      card++;
    }*/
    //stat.totalWeight = totWeight;
    //stat.card = card;
    //std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<cumWeight<<" "<<card/2<<" "<<stat.edgeRetained<<" "<<stat.streamReadTime<<" "<<stat.streamProcessTime<<" "<<stat.streamReadProcessTime<<" "<<stat.streamPostProcessTime<<" "<<stat.colTime<<std::endl;

    if(allFlags.dp) { 
      std::vector<EndPoint> matchMerge(stat.n, {-1,-1});

      for(NODE_T i=0;i<k;i++) {
        mergeDJMatch(stat,matchOut[i],matchOut[2*k-i-1],matchMerge); 
        matchOut[i] = matchMerge;
        matchMerge.clear();
        matchMerge.resize(stat.n,{-1,-1});
      }
    }
      
      /*for(NODE_T i=0;i<stat.n;i++) {
        if(matchOut[0][i].v != -1 && matchMerge[i].v == -1) {
          auto mate0 = matchOut[0][i].v; 

          if(matchMerge[mate0].v == -1) {
            matchMerge[i] = matchOut[0][i];
            matchMerge[mate0] = matchOut[0][mate0]; 
          }
        } 
      }*/
      
    std::cout<<"\n*************************************************************"<<std::endl;
    cumWeight = 0.0; 
    EDGE_T totCard = 0;
    for(NODE_T i =0;i<k;i++) {
      card = 0;
      totWeight = 0.0;
      for(NODE_T j =0; j<stat.n;j++) {
        if(matchOut[i][j].v != -1) {
          totWeight += static_cast<SUM_T> (matchOut[i][j].weight);       
          card++;
        } 
      } 
      std::cout<<std::fixed<<"Matching: "<<i<<" "<<"Weight: "<<totWeight/2<<" "<<"Cardinality: "<<card/2<<std::endl;
      cumWeight += totWeight;
      totCard += card;
    }
    std::cout<<"\n*************************************************************"<<std::endl;
    cumWeight = cumWeight/2;
      
      /*for(auto e:matchOut.matchedEdges) {
        //std::cout<<e.u<<" "<<e.v<<" "<<e.weight<<"\n";
        totWeight += static_cast<SUM_T> (e.weight);
        card++;
      }*/
      //stat.totalWeight = totWeight;
      //stat.card = card;
      //std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<cumWeight<<" "<<card/2<<" "<<stat.edgeRetained<<" "<<stat.streamReadTime<<" "<<stat.streamProcessTime<<" "<<stat.streamPostProcessTime<<" "<<stat.colTime<<" "<<stat.dpTime<<std::endl;

    
    stat.mem = getPeakRSS()-baseline;
    //std::cout<<"mem: "<<stat.mem<<std::endl;
    stat.card = totCard/2;
    stat.totalWeight = cumWeight;
    stat.simStream = sim;
    printResults(getLastPartOfFilepath(fname),alg,stat,epsilon,allFlags);
    if(resFile.empty() == false)
      writeResults(getLastPartOfFilepath(fname),k,algName,stat,epsilon);

  }
  /*
  totWeight = 0.0;
  card = 0;
  for(NODE_T j =0; j<stat.n;j++) {
    if(matchMerge[j].v != -1) {
      totWeight += static_cast<SUM_T> (matchMerge[j].weight);       
      card++;
    } 
  } 
  std::cout<<"New Weight: "<<totWeight/2<<" New Card.: "<<card/2<<std::endl;

  std::vector<bool> isMatch(stat.n, false);

  for(NODE_T i=0;i<stat.n;i++) {
    if(matchMerge[i].v != -1) {
      if(isMatch[i] == false) {
        auto mate = matchMerge[i].v;
        if(isMatch[mate] == true) {
          std::cout<<"invalid1"<<std::endl;
          break; 
        }
        isMatch[i] = true;
        isMatch[mate] = true;
      }
      else {
        auto mate = matchMerge[i].v;
        if(isMatch[mate] == false) {
          std::cout<<"invalid2"<<std::endl;
          break; 
        }
      }
    } 
  }
  
  std::vector<bool> isMatch1(stat.n, false);

  for(NODE_T i=0;i<stat.n;i++) {
    if(matchOut[1][i].v != -1) {
      if(isMatch1[i] == false) {
        auto mate = matchOut[1][i].v;
        if(isMatch1[mate] == true) {
          std::cout<<"invalid3"<<std::endl;
          break; 
        }
        isMatch1[i] = true;
        isMatch1[mate] = true;
      }
      else {
        auto mate = matchOut[1][i].v;
        if(isMatch1[mate] == false) {
          std::cout<<"invalid4"<<std::endl;
          break; 
        }
      }
    } 
  }
  */
  return 0;
}
