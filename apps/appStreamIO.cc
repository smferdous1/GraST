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
#include "cxxopts/cxxopts.hpp"
#include "GraST/MemUsage.h"
#include "GraST/Utility.h"

#include <omp.h>

#include<iostream>
#include <filesystem>

std::fstream fWrite;

void writeResults(std::string graphName, NODE_T n, EDGE_T m, double read_time) {
  fWrite<<graphName<<","<<n<<","<<m<<","<<read_time<<std::endl;
}

int main(int argC, char *argV[]) {

  cxxopts::Options options("stio", "streaming input reading time test");
  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("out,res_out","output results", cxxopts::value<std::string>()->default_value(""))
    ("h,help", "print usage")
    ;
  std::string fname,resFile;
  try { 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    fname = result["file"].as<std::string>();
    resFile = result["res_out"].as<std::string>();
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
      fWrite<<"graph,n,m,read-time"<<std::endl;
  }
  TrueStreamIn sio(fname);
  
  //std::cout<<streamIn.getNumberOfEdg()<<std::endl;
  WeightEdgeSim we;
  
  double t1 = omp_get_wtime(); 
  while(sio.nextElement(we)){
    //std::cout<<we.v+1<<" "<<we.u+1<<" "<<we.weight<<std::endl;
  }
  double t = omp_get_wtime() - t1;

  std::string graph_name = getLastPartOfFilepath(fname);
  NODE_T n = sio.getNumberOfVert();
  NODE_T m = sio.getNumberOfEdg();

  std::cout<<"graph :"<<graph_name<<std::endl;
  std::cout<<"Num Vertex :"<<n<<std::endl;
  std::cout<<"Num Edges :"<<m<<std::endl;
  std::cout<<"Read Time :"<<t<<std::endl;

  if(resFile.empty() == false)
    writeResults(graph_name,n,m,t);



  return 0;
}
