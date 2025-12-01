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


#include "GraST/Match.h"
#include "cxxopts/cxxopts.hpp"

#include<iostream>
#include<vector>
#include<stdlib.h>
#include<omp.h>

int main(int argC, char *argV[]) {
  
  //std::vector<NODE_T> path = {0,1,2,3,4,5};
  //std::vector<VAL_T> weights = {1,5,2,10,5};
  
  Stat stat;
  stat.n = 5; 
  std::vector<EndPoint> match1 = {{1,10},{0,10},{3,10},{2,10},{-1,-1}};
  std::vector<EndPoint> match2 = {{-1,-1},{2,5},{1,5},{4,20},{3,20}};
  std::vector<EndPoint> matchOut(5,{-1,-1});
  //DPPathOpt(path,weights,match);
  mergeDJMatch(stat,match2,match1,matchOut);

  for(NODE_T i=0;i<5;i++) {
    std::cout<<i<<" "<<matchOut[i].v<<" "<<matchOut[i].weight<<std::endl;
  }
  return 0;
}
