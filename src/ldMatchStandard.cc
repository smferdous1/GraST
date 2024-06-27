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
#include <omp.h>

#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>

NODE_T bestMate( const NODE_T u, std::vector<bool> const &unavail, Adjacents const & adj, std::vector<NODE_T > const &mate) {
  
  NODE_T start = mate[u];
  for (; start < adj.size(); ++start ) {
    if ( unavail[adj[start].v] == false) {
      //return {rv->v,rv->id,rv->weight};
      return start;
    } 
  }
  return start;

}

MatchOutLite ldMatching(const NODE_T n, AdjacencyList const &adjList) {
  
  //WeightEdgeSimList matchedEdges;

  //To track a vertex v is already matched or all the neighbors of v is matched.
  std::vector<bool> unavail(n,false);
  std::vector<NODE_T> mates(n,0);

  //matchedEdges.clear();
  
  int iteration = 1;
  std::vector<NODE_T> vtxToProcess(n),vtxToProcessNext;
  std::iota(vtxToProcess.begin(),vtxToProcess.end(),0);

  std::vector<NODE_T> &currList = vtxToProcess;
  std::vector<NODE_T> &nextList = vtxToProcessNext;

  double postProcessStart = omp_get_wtime();
  while(currList.empty() == false) {
    
    nextList.clear();
    //flag_done = true;
    for(NODE_T u: currList) {
      if (unavail[u] == false) {

        //double t_mate = omp_get_wtime();
        mates[u] = bestMate(u,unavail,adjList[u],mates); 
        //t_mate_tot = t_mate_tot + (omp_get_wtime()-t_mate);
        if (mates[u]<adjList[u].size())
        {
          NODE_T v = adjList[u].at(mates[u]).v;

          //t_mate = omp_get_wtime();
          mates[v] = bestMate(v,unavail,adjList[v],mates); 
          //t_mate_tot = t_mate_tot + (omp_get_wtime()-t_mate);

          NODE_T u_tmp;
          if ( mates[v] < adjList[v].size()  ) {
            u_tmp = adjList[v].at(mates[v]).v;
            //both u and v points to each other.
            if ( u_tmp == u)
            {
              unavail[u] = true;
              unavail[v] = true;
              //flag_done = false;
            }
            else
              nextList.push_back(u);
            
          }
          //No neighbor of u_tmp.v is available to match.
          else
            unavail[v] = true;
        }
        //No neighbor of i is available to match.
        else
          unavail[u] = true;
      }
    }  
    std::swap(currList,nextList);
    iteration++;
  }
  
  double postProcessEnd = omp_get_wtime();
  SUM_T totalWeight{};
  NODE_T card{};
  //constructing the matching
  for ( NODE_T u=0;u<n;u++) {
    if (mates[u] != adjList[u].size() ) {
        auto v = adjList[u][mates[u]].v; 
        if (u<v) {
          auto w = adjList[u][mates[u]].weight; 
          totalWeight = totalWeight + w;
          card++;
          //matchedEdges.push_back({u,v,w});
          //std::cout<<u<<" "<<v<<" "<<w<<"\n";
      }
    } 
  }

  return {totalWeight,card,n,0,0,postProcessEnd-postProcessStart};
}
