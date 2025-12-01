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



#include<GraST/StreamMatch.h>
#include<omp.h>

#include<iostream>
#include<cassert>

#include<stack>
#include<vector>
#include<algorithm>
#include<chrono>

#define ASSERT 1

//Template Definitions
template void StreamMatch::MatchFb<TrueStreamIn>(std::vector<EndPoint> &, Stat &, float const = 0.001);
template void StreamMatch::MatchFb<StreamIn>(std::vector<EndPoint> &, Stat &, float const = 0.001);
template void StreamMatch::MatchFb<FeatureMatStream>(std::vector<EndPoint> &, Stat &, float const = 0.001);

template void StreamMatch::MatchGW<TrueStreamIn>(std::vector<WeightEdgeSim> &, Stat &, float const = 0.001, VAL_T=0,bool=false);
template void StreamMatch::MatchGW<StreamIn>(std::vector<WeightEdgeSim> &, Stat &, float const = 0.001, VAL_T = 0,bool=false);
template void StreamMatch::MatchGW<FeatureMatStream>(std::vector<WeightEdgeSim> &, Stat &, float const = 0.001, VAL_T = 0,bool=false);

template void StreamMatch::MatchGW<TrueStreamIn>(std::vector<WeightEdgeSim> &, std::vector<EndPoint> &, std::vector<bool> &, Stat &, float = 0.001,bool=false);
template void StreamMatch::MatchGW<StreamIn>(std::vector<WeightEdgeSim> &, std::vector<EndPoint> &, std::vector<bool> &, Stat &, float = 0.001,bool=false);
template void StreamMatch::MatchGW<FeatureMatStream>(std::vector<WeightEdgeSim> &, std::vector<EndPoint> &, std::vector<bool> &, Stat &, float = 0.001,bool=false);

template KMatchOut StreamMatch::kDMatchGW<TrueStreamIn>(Stat &stat, NODE_T k, bool maximal, float const eps); 
template KMatchOut StreamMatch::kDMatchGW<StreamIn>(Stat &stat, NODE_T k, bool maximal, float const eps); 

bool streamProcessGW(WeightEdgeSim &we,  std::stack <WeightEdgeSim> &eStack_i, 
    std::vector<VAL_T> &pot, float eps, bool tight) {

  if (we.weight < (1+eps)*(pot[we.u] + pot[we.v]))
    return false;

  VAL_T wPrime = we.weight - (pot[we.u] + pot[we.v]);
  eStack_i.push(we);

  //dual updates can be regular or 1/2*wPrime w/o affecting approx. ratio.
  if(tight == false) {
    pot[we.u] += wPrime;
    pot[we.v] += wPrime;
  }
  else {
    pot[we.u] += 0.5*wPrime;
    pot[we.v] += 0.5*wPrime;
  }
  return true;
  
} 


//overloaded for edge weight offset to work with edge cover
 bool streamProcessGW(WeightEdgeSim &we,  std::stack <WeightEdgeSim> &eStack_i, 
    std::vector<VAL_T> &pot, float eps, VAL_T offset, bool tight) {
  
  if(offset-we.weight < 0 ) return false;

  if ((offset - we.weight) < (1+eps)*(pot[we.u] + pot[we.v]))
    return false;

  VAL_T wPrime = (offset-we.weight) - (pot[we.u] + pot[we.v]);
  eStack_i.push(we);

  if(tight == false) {
    pot[we.u] += wPrime;
    pot[we.v] += wPrime;
  }
  else {
    pot[we.u] += 0.5*wPrime;
    pot[we.v] += 0.5*wPrime;
  }
  return true;
} 

template<class SIn>
void StreamMatch::MatchFb(std::vector<EndPoint> &matching, Stat &stat, float const eps) {
  
  WeightEdgeSim we;

  double t1 = omp_get_wtime();
  SIn sio(fileName);
  stat.streamReadTime = omp_get_wtime()-t1;
  NODE_T n = sio.getNumberOfVert();
  matching.resize(n,{-1,0});

  stat.streamProcessTime = 0.0;

  double tmpReadplusProcess = omp_get_wtime();
  while(sio.nextElement(we)) {
    auto u = we.u;
    auto v = we.v;
    auto weight = we.weight; 

    VAL_T sumNeighbor = 0;
    if (u < v && weight > 0) {
      if(matching[u].v != -1) {
        sumNeighbor = sumNeighbor + matching[u].weight; 
      }
      if(matching[v].v != -1) {
        sumNeighbor = sumNeighbor + matching[v].weight; 
      }

      if(weight > 2*sumNeighbor) {
        matching[u] = {v,weight}; 
        matching[v] = {u,weight}; 
      }
    }
  }   
  stat.streamReadProcessTime = omp_get_wtime() - tmpReadplusProcess;

  stat.n = sio.getNumberOfVert();
  stat.m = sio.getNumberOfEdg();
}

//offset only need for feature  matrix 
template<class SIn>
void StreamMatch::MatchGW(std::vector<WeightEdgeSim> &matchEdges,
    Stat &stat, float const eps, VAL_T offset, bool tight) {
  WeightEdgeSim we;
  double t1 = omp_get_wtime();
  SIn sio(fileName,offset);
  stat.streamReadTime = omp_get_wtime()-t1;
  NODE_T n = sio.getNumberOfVert();
  std::stack<WeightEdgeSim> eStack;
  std::vector<VAL_T> potentials(n, 0);

  stat.streamProcessTime = 0.0;

  double tmpReadplusProcess = omp_get_wtime();
  while (sio.nextElement(we)) {
    if (we.u < we.v && we.weight > 0) {
      streamProcessGW(we,eStack,potentials,eps,tight);
    }

  }
  stat.streamReadProcessTime = omp_get_wtime() - tmpReadplusProcess;

  t1 = omp_get_wtime();
  std::vector<bool> matched(n, false);
  stat.stackSize = eStack.size();
  stat.edgeRetained = stat.stackSize;
  stat.n = sio.getNumberOfVert();
  stat.m = sio.getNumberOfEdg();

  auto stackSize = eStack.size();

  //post-processing: maximal matching by unwinding stack
  while (!eStack.empty()) {
    WeightEdgeSim topEdge = eStack.top();
    eStack.pop();
    NODE_T u = topEdge.u;
    NODE_T v = topEdge.v;

    if (matched[u] == false && matched[v] == false) {
      matchEdges.push_back(topEdge);
      matched[u] = true;
      matched[v] = true;
    }
  }
  double t2 = omp_get_wtime();
  stat.streamPostProcessTime = t2-t1;

  if (ASSERT == 1)
    assert(isValidMatching(n, matchEdges) == 1);

  //calculate an upperbound
  VAL_T sumPot=0;
  for( auto p:potentials) {
    sumPot = sumPot + p;  
  }

  if(tight)
    stat.ub = sumPot*(1+eps);
  else
    stat.ub =sumPot*(1+eps);
}

// overloaded method to work on offset
template<class SIn>
void StreamMatch::MatchGW(std::vector<WeightEdgeSim> &matchEdges, 
    std::vector<EndPoint> &offset, std::vector<bool> &matched, Stat &stat, 
    float eps, bool tight) {

  WeightEdgeSim we;
  double t1 = omp_get_wtime();
  SIn sio(fileName);
  stat.streamReadTime = omp_get_wtime()-t1;
  NODE_T n = sio.getNumberOfVert();
  std::stack<WeightEdgeSim> eStack;
  std::vector<VAL_T> potentials(n, 0);
  
  double tmpReadplusProcess = omp_get_wtime();
  while (sio.nextElement(we)) {
    if (we.u < we.v && we.weight > 0){
      VAL_T offsetSum = offset[we.u].weight + offset[we.v].weight;
      streamProcessGW(we,eStack,potentials,eps,offsetSum);
    }
  }

  stat.streamReadProcessTime = omp_get_wtime() - tmpReadplusProcess;
  // unwinding stack
  matched.resize(n, false);
  stat.stackSize = eStack.size();
  stat.edgeRetained = stat.stackSize;
  stat.n = sio.getNumberOfVert();
  stat.m = sio.getNumberOfEdg();

  t1 = omp_get_wtime();
  while (!eStack.empty()) {
    WeightEdgeSim topEdge = eStack.top();
    eStack.pop();
    NODE_T u = topEdge.u;
    NODE_T v = topEdge.v;
    //VAL_T w = topEdge.weight;

    if (matched[u] == false && matched[v] == false) {
      matchEdges.push_back(topEdge);
      matched[u] = true;
      matched[v] = true;
    }
  }
  double t2 = omp_get_wtime();
  stat.streamPostProcessTime = t2-t1;
}


template<class SIn>
KMatchOut StreamMatch::kDMatchGW(
    Stat &stat, NODE_T k, bool maximal, float const eps) {


  WeightEdgeSim we;
  double t1 = omp_get_wtime();
  SIn sio(fileName);
  stat.streamReadTime = omp_get_wtime()-t1;

  NODE_T n = sio.getNumberOfVert();
  std::vector<std::stack<WeightEdgeSim> > eStack(k);
  std::vector<std::vector<VAL_T> > potentials(k, std::vector<VAL_T> (n,0));
  //std::vector<VAL_T> minPot(n,0.0);
  std::vector<WeightEdgeSimList > edgeList(k);


  stat.streamProcessTime = 0.0;

  std::vector<std::vector<bool> > isAvail; //(k,std::vector<bool> (n,true));

  if (maximal == true) {
    isAvail.resize( k,std::vector<bool> (n,true));
  }

  //auto start = std::chrono::high_resolution_clock::now();
  double tmpReadplusProcess = omp_get_wtime();
  while (sio.nextElement(we)) {
    //t1 = omp_get_wtime();

    //if((1+eps)*(minPot[we.u]+minPot[we.v]) > we.weight) {
    //  continue;
    //}


    if (we.u < we.v && we.weight > 0) {
      bool success = false;  
      for(NODE_T i =0;i<k;i++) {
        //NODE_T firstInd = sio.getNumberOfEdg() % k;
        //NODE_T firstInd = 0;
        success = streamProcessGW(we,eStack[i],potentials[i],eps,false); 
        if(success == true) {
          /*if(minPot[we.u] == 0.0 || (minPot[we.u] > potentials[i][we.u])) {
            minPot[we.u] = potentials[i][we.u];
            }
            if(minPot[we.v] == 0.0 || (minPot[we.v] > potentials[i][we.v])) {
            minPot[we.v] = potentials[i][we.v];
            }*/
          stat.edgeRetained++;
          break;
        }
      }

      if(maximal == true && success == false) {
        for(NODE_T i=0;i<k;i++) {
          if(isAvail[i][we.u] == true && isAvail[i][we.v] == true) {
            edgeList[i].push_back(we); 
            isAvail[i][we.u] = false;
            isAvail[i][we.v] = false;
          } 
        }
      }
    }

    //stat.streamProcessTime += (omp_get_wtime() - t1);
  }

  //stat.streamReadTime = (omp_get_wtime() - tmpReadplusProcess) - stat.streamProcessTime;
  stat.streamReadProcessTime = omp_get_wtime() - tmpReadplusProcess;
  stat.n = sio.getNumberOfVert();
  stat.m = sio.getNumberOfEdg();

  // unwinding stack
  std::vector<std::vector<bool> > matched(k, std::vector<bool> (n,false));
  t1 = omp_get_wtime();
  EDGE_T stackSize = 0;
  KMatchOut match(k,std::vector<EndPoint> (n,{-1,-1}));

  for(NODE_T i=0;i<k;i++) {
    stackSize = stackSize + eStack[i].size();
    while (!eStack[i].empty()) {
      WeightEdgeSim topEdge = eStack[i].top();
      eStack[i].pop();
      NODE_T u = topEdge.u;
      NODE_T v = topEdge.v;
      VAL_T w = topEdge.weight;

      if (matched[i][u] == false && matched[i][v] == false) {
        //std::cout<<u<<","<<v<<","<<w<<"\n";
        match[i][u] = {v,w};
        match[i][v] = {u,w};
        matched[i][u] = true;
        matched[i][v] = true;
      }
      else {
        bool success = false;
        for(NODE_T j=i+1; j<k;j++) {
          success = streamProcessGW(topEdge,eStack[j],potentials[j],eps,false); 

          if(success == true)
            break;
        } 
        if(maximal == true && success == false) {
          for(NODE_T j=i;j<k;j++) {
            if(isAvail[j][we.u] == true && isAvail[j][we.v] == true) {
              edgeList[j].push_back(we); 
              isAvail[j][we.u] = false;
              isAvail[j][we.v] = false;
            } 
          }
        }
      }

    }
  }

  if(maximal == true) {
    for(NODE_T i =0;i<k;i++) {
      for( auto e:edgeList[i]) {
        if(matched[i][e.u] == false && matched[i][e.v] == false) {
          match[i][e.u] = {e.v,e.weight}; 
          match[i][e.v] = {e.u,e.weight}; 
          matched[i][e.u] = true;
          matched[i][e.v] = true;
        } 
      }
    }
  }
  stat.stackSize = stackSize;
  double t2 = omp_get_wtime();
  stat.streamPostProcessTime = t2-t1;
  /*if (ASSERT == 1)
    assert(isValidMatching(n, matchEdges) == 1);*/

  return match;
}
