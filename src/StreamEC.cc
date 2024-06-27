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


#include<GraST/StreamEC.h>

#include<omp.h>
#include<iostream>
#include<limits>
#include<vector>


template<class SIn>
void StreamEC::ECPD(std::vector<EndPoint> &edgePointers, Stat &stat) {
  WeightEdgeSim we;

  double t1 = omp_get_wtime();
  SIn sio(fileName);
  stat.streamReadTime = omp_get_wtime() - t1; 

  NODE_T n = sio.getNumberOfVert();
  std::vector<VAL_T> potentials;
  potentials.resize(n, -1);

  EndPoint ep;
  ep.v = 0;
  ep.weight = -1;
  edgePointers.resize(n, {0, -1});

  std::vector<char> t;
  t.resize(n, -1);

  std::vector<EndPoint> mu;
  mu.resize(n, {0, -1});

  EndPoint ep1;
  double tmpReadProcess = omp_get_wtime();
  while (sio.nextElement(we)) {
  
    NODE_T u = we.u;
    NODE_T v = we.v;
    VAL_T weight = we.weight;

    if (u >= v || weight <= 0)
      continue;
    ep.v = v;
    ep.weight = weight;

    if (mu[u].weight == -1 || weight < mu[u].weight)
      mu[u] = {v, weight};

    // making sure that the first vertex is the index vertex
    if (mu[v].weight == -1 || weight < mu[v].weight) {
      mu[v] = {u, weight};
    }


    if ((potentials[u] < 0 || (weight/2.0 <= potentials[u])) &&
        (potentials[v] < 0 || (weight/2.0 <= potentials[v]))) {

      if (t[u] == 2) {
       t[edgePointers[u].v] = 0;
       potentials[edgePointers[u].v] = mu[edgePointers[u].v].weight;
      }
      if (t[v] == 2) {
       t[edgePointers[v].v] = 0;
       potentials[edgePointers[v].v] = mu[edgePointers[v].v].weight;
      }

      ep1.v = u;
      ep1.weight = weight;

      edgePointers[u] = ep;
      edgePointers[v] = ep1;
      potentials[u] = weight/2.0;
      potentials[v] = weight/2.0;
      t[u] = 2;
      t[v] = 2;
    }
    else if ( (potentials[u] < 0) || (weight < potentials[u]) ) {
      // std::cout<<"covering by 1"<<std::endl;

      edgePointers[u] = mu[u];
      potentials[u] = mu[u].weight;
      t[u] = 1;
    }
    else if ( (potentials[v] < 0) || (weight < potentials[v]) ) {
      // std::cout<<"covering by 1"<<std::endl;

      ep1.v = u;
      ep1.weight = weight;

      edgePointers[v] = mu[v];
      potentials[v] = mu[v].weight;
      t[v] = 1;
    }
    // std::cout<<std::endl;
  }
  stat.streamReadProcessTime = omp_get_wtime()-tmpReadProcess;

  t1 = omp_get_wtime();
  // post processing
  for (NODE_T i=0; i < n; i++) {
    if (t[i] == 0 || t[i] == 1) {
      // std::cout<<i<<" "<<t[i]<<std::endl;
      edgePointers[i] = mu[i];
    }
  }
  stat.streamPostProcessTime = omp_get_wtime() - t1;
  stat.n = sio.getNumberOfVert();
  stat.m = sio.getNumberOfEdg();
}

template<class SIn>
void StreamEC::ECNN(std::vector<EndPoint> &nnEdge, Stat &stat) {
  WeightEdgeSim we;
  double t1 = omp_get_wtime();
  SIn sio(fileName);
  stat.streamReadTime = omp_get_wtime() - t1;

  NODE_T n = sio.getNumberOfVert();
  nnEdge.resize(n, {0, -1});
    
  
  double tmpReadProcess = omp_get_wtime();
  while (sio.nextElement(we)) {
    if (we.u >= we.v || we.weight == 0)
      continue;

    if (nnEdge[we.u].weight == -1 || we.weight < nnEdge[we.u].weight) {
      nnEdge[we.u] = {we.v, we.weight};
    }

    if (nnEdge[we.v].weight == -1 || we.weight < nnEdge[we.v].weight) {
      // make sure first vertex matches with the index always.
      nnEdge[we.v] = {we.u, we.weight};
    }
  }

  stat.streamReadProcessTime = omp_get_wtime()-tmpReadProcess;
  stat.n = sio.getNumberOfVert();
  stat.m = sio.getNumberOfEdg();
}

template<class SIn>
void StreamEC::TwoPass(WeightEdgeSimList &edgeCover, Stat &stat, float eps, bool sim) {
  std::vector<EndPoint> nnEdge;
  Stat stat1;

  // Pass 1: NN
  ECNN<SIn>(nnEdge, stat1);

  // Pass 2: Matching
  std::vector<bool> matched;
  StreamMatch stmatch(fileName);
  stmatch.MatchGW<SIn>(edgeCover, nnEdge, matched, stat, eps);

  // post processing: assign the nearest neighbor edge to
  // any uncovered vertices (except the singular vertices)
  double t1 = omp_get_wtime();
  for (NODE_T i=0; i < stat1.n; i++) {
    if (matched[i] == false && nnEdge[i].weight > 0) {
      edgeCover.push_back({i, nnEdge[i].v, nnEdge[i].weight});
    }
  }

  double ppTime = omp_get_wtime() - t1;

  //find all the time
  stat.streamReadTime = stat.streamReadTime + stat1.streamReadTime;
  stat.streamReadProcessTime = stat.streamReadProcessTime + stat1.streamReadProcessTime;
  stat.streamPostProcessTime = stat.streamPostProcessTime + stat1.streamPostProcessTime+ppTime;
}

 
template void StreamEC::ECPD<TrueStreamIn>(std::vector<EndPoint> &, Stat &);
template void StreamEC::ECPD<StreamIn>(std::vector<EndPoint> &, Stat &);
template void StreamEC::ECNN<TrueStreamIn>(std::vector<EndPoint> &, Stat &);
template void StreamEC::ECNN<StreamIn>(std::vector<EndPoint> &, Stat &);
template void StreamEC::TwoPass<TrueStreamIn>(WeightEdgeSimList &edgeCover, Stat &stat, float = 0.001, bool sim =false);
template void StreamEC::TwoPass<StreamIn>(WeightEdgeSimList &edgeCover, Stat &stat, float = 0.001, bool sim =false);
