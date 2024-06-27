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

#include<iostream>
#include<vector>

int main(int argC, char *argV[]) {

  cxxopts::Options options("streamEC", "Streaming edgecover algorithms ");

  options.add_options()
    ("f,file", "file name", cxxopts::value<std::string>())
    ("a,algorithm", "algo name: nn ecpd twopass", cxxopts::value<std::string>()->default_value("ecpd"))
    ("e,eps", "epsilon value", cxxopts::value<float>()->default_value("0.001"))
    ("h,help", "print usage")
    ;
  std::string fname,alg;
  float epsilon;
  try{ 
    auto result = options.parse(argC,argV);
    if (result.count("help")) {
          std::cout<< options.help()<<"\n"; 
          std::exit(0);
    }
    fname = result["file"].as<std::string>();
    alg = result["algorithm"].as<std::string>();
    epsilon = result["eps"].as<float>();
  }
  catch(cxxopts::exceptions::exception &exp) {
    std::cout<<options.help()<<std::endl;
    exit(1);
  }

  StreamEC sec(fname);
  std::vector<WeightEdgeSim> edgeCover;
  Stat stat;
  sec.TwoPass<TrueStreamIn>(edgeCover,stat,epsilon);

  //Match Stats
  SUM_T totWeight = 0.0;
  NODE_T card=0;
  for(auto e:edgeCover) {
    //std::cout<<e.u<<" "<<e.v<<" "<<e.weight<<std::endl;
    totWeight += e.weight;
    card++;
  }
  stat.totalWeight = totWeight;
  stat.card = card;
  std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<stat.totalWeight<<" "<<stat.card<<" "<<stat.stackSize<<std::endl;
  return 0;
}
