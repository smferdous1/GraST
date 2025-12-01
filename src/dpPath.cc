/*
 * Copyright (C) 2022  Ferdous,S M <ferdous.csebuet@egmail.com>
 * Author: Ferdous,S M <ferdous.csebuet@egmail.com>
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
//#include "spdlog/spdlog.h"

#include <iostream>
#include <algorithm>
#include <omp.h>

void mergeDJMatch(Stat &stat, std::vector<EndPoint> &match1, std::vector<EndPoint> &match2,
    std::vector<EndPoint> &matchOut) {

  NODE_T n = stat.n; 
  std::vector<bool> used(n,false); 
  std::vector<NODE_T> path;
  std::vector<VAL_T> weights;
  

  NODE_T nPath = 0;
  
  double t1 = omp_get_wtime();
  for(NODE_T i=0;i<n;i++) {
    if(used[i] == false) {
      path.clear();
      weights.clear();
      if((match1[i].v != -1) && (match2[i].v == -1)) {
        used[i] = true;
        auto current = match1[i];
        path.push_back(i); 
        weights.push_back(match1[i].weight);

        while(true) {
          used[current.v] = true;
          path.push_back(current.v); 
          if(match2[current.v].v == -1)
            break;
          weights.push_back(match2[current.v].weight);

          current = match2[current.v];
          used[current.v] = true;
          path.push_back(current.v); 
          if(match1[current.v].v == -1)
            break;
          weights.push_back(match1[current.v].weight);

          current = match1[current.v];

        }
        nPath += path.size();
        DPPathOpt(path,weights,matchOut);

      } 
      else if((match1[i].v == -1) && (match2[i].v != -1)) {
              
        used[i] = true;
        auto current = match2[i];
        path.push_back(i); 
        weights.push_back(match2[i].weight);

        while(true) {
          used[current.v] = true;
          path.push_back(current.v); 
          if(match1[current.v].v == -1)
            break;
          weights.push_back(match1[current.v].weight);

          current = match1[current.v];
          used[current.v] = true;
          path.push_back(current.v); 
          if(match2[current.v].v == -1)
            break;
          weights.push_back(match2[current.v].weight);

          current = match2[current.v];

        }
        nPath += path.size();
        DPPathOpt(path,weights,matchOut);
      }

      else if(match1[i].v == -1 && match2[i].v == -1) {
        used[i] = true; 
      }
    } 
  } 
  //std::cout<<nPath<<std::endl;

  //for cycle
  NODE_T nCycle = 0;
  for(NODE_T i=0;i<n;i++) {
    if(used[i] == true) continue; 
    auto current = i;
    path.clear();
    weights.clear();
    while(used[current] == false) {
      used[current] = true;
      path.push_back(current); 
      weights.push_back(match1[current].weight);

      current = match1[current].v;

      used[current] = true;
      path.push_back(current);
      weights.push_back(match2[current].weight);

      current = match2[current].v;
    }
    nCycle++;
    path.push_back(i);
    DPCycleOpt(path,weights,matchOut);
  }
  //std::cout<<nCycle<<std::endl;
  stat.dpTime = omp_get_wtime() - t1;
}

void DPPathOpt(std::vector<NODE_T> &path, std::vector<VAL_T> &weights, 
   std::vector<EndPoint> &match) { 
  
  NODE_T l = path.size();
  
  //whether the edge is in the matching or not 
  std::vector<bool> in(l-1,false);

  VAL_T wOld =0;
  VAL_T wNew = weights[0];
  in[0] = true;

  for( NODE_T i=1;i<l-1;i++) {
    if(weights[i] + wOld > wNew) { 
      in[i] = true;
      auto temp = wNew;
      wNew = weights[i] + wOld;
      wOld = temp;
    }
    else {
      in[i] = false;
      wOld = wNew;
    }
  }
  
  NODE_T k = l-2;
  while(k>=0) {
    if(in[k] == true) {
      match[path[k]] = {path[k+1],weights[k]};
      match[path[k+1]] = {path[k],weights[k]};
      k = k-2;
    }
    else {
      k = k - 1;
    }
  }
}


void DPCycleOpt(std::vector<NODE_T> &path, std::vector<VAL_T> &weights, 
   std::vector<EndPoint> &match) { 
  
  NODE_T l = path.size();
  
  //whether the edge is in the matching or not 
  std::vector<bool> in1(l-1,false);
  std::vector<bool> in2(l-1,false);

  VAL_T wOld1 = 0;
  VAL_T wNew1 = weights[0];
  in1[0] = true;

  VAL_T wOld2 = 0;
  VAL_T wNew2 = weights[1]; //ignore the first edge
  in2[0] = true;
  

  for( NODE_T i=1;i<l-2;i++) {
    //first path: ingore the last edge
    if(weights[i] + wOld1 > wNew1) { 
      in1[i] = true;
      auto temp = wNew1;
      wNew1 = weights[i] + wOld1;
      wOld1 = temp;
    }
    else {
      in1[i] = false;
      wOld1 = wNew1;
    }
    
    //second path: ignores the first edge
    if(weights[i+1] + wOld2 > wNew2) { 
      in2[i] = true;
      auto temp = wNew2;
      wNew2 = weights[i+1] + wOld2;
      wOld2 = temp;
    }
    else {
      in2[i] = false;
      wOld2 = wNew2;
    }

  }
  
  NODE_T k = l-3;
  while(k>=0) {
    if(wNew1>wNew2) {
      if(in1[k] == true) {
        match[path[k]] = {path[k+1],weights[k]};
        match[path[k+1]] = {path[k],weights[k]};
        k = k-2;
      }
      else {
        k = k - 1;
      }
    }
    else {
      if(in2[k] == true) {
        match[path[k+1]] = {path[k+2],weights[k+1]};
        match[path[k+2]] = {path[k+1],weights[k+1]};
        k = k-2;
      }
      else {
        k = k - 1;
      }
    }
  }
}
