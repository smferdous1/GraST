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

/** An implementation of the parallel locally dominant matching using phase idea. Each iteration
 * runs in two phases: 1) computing the best mate for the candidate vertices and update the 
 * candidate set of edges to be looked into the phase 2. 2) Check for edges who are locally dominant
 * in the candidate set, add them to mathcing, and construct the candidate set of vertices for phase 1 in 
 * next iteration. 
 *      @param n number of nodes in the graph
 *      @param adjList Graph is in adjacency list format. The neighboring edges of a vertex is sorted in 
 *             order of priority. Each edge is stored twice for each endpoint
 *      
 *      @return A structure containing total weights of matched edges and a bunch of stats. See MatchoutLite struct in Match.h
 *
 */
MatchOutLite ldMatchingTwoPhasesPar( const NODE_T n, AdjacencyList const & adjList) {

  
  std::vector<NODE_T> mates(n,0);     //for all vertex the best partner is the first one since the adjacency list is sorted
  
  std::vector<bool> available(n,true); 



  //for debug  
 /* 
  for(auto u=0;u<n;u++) {
    for (auto e:adjList[u]) {
      std::cout<<u<<" "<<e.v<<" "<<e.id<<"\n";
    } 
  }
  */
   
  
  double postProcessStart = omp_get_wtime();
  
  bool flag_done = false;
  while (flag_done == false) {
    /*************************Phase 2*****************************************************/ 
    //std::cout<<"Phase 1"<<"\n";
    //double t1P2 = omp_get_wtime();
    flag_done = true;

    #pragma omp parallel for default(none) shared(adjList,mates,available,n)
    for (NODE_T u =0;u<n;u++) {
      if (available[u] == true) { 

        auto mateUind = mates[u];
        if (mateUind == adjList[u].size()) {
          continue; 
        }
        //std::cout<<u<<" "<<mates[u]<<std::endl;
        NODE_T v = adjList[u][mateUind].v;   //u points to v; v is the best partner for u
        if (u > v) {
          //candItemsP1.push_back(u);
          continue;
        }

        if (available[v] == true) {
          auto mateVind = mates[v];
          auto mateV = adjList[v][mateVind].v;  //v points to mateV

          if ( mateV == u ) {        // both u and v points to each other, so (u,v) is a locally dominant edge.
            available[u] = false;
            available[v] = false;

          }
          /*
          else {
             candItemsP1.push_back(u); 
          }
          */
        }
        //else candItemsP1.push_back(u); 

      }
    } 
    //std::cout<<"Phase 2 timing and work "<<omp_get_wtime() - t1P2<<" "<<candItemsP2.size()<<"\n";

    //std::cout << "Phase 2"<<"\n";
    
    /***********************Phase 1*****************************************************/
    //double t1P1 = omp_get_wtime();
    
    //candItemsP2.clear(); 
    #pragma omp parallel for default(none) shared(available,adjList,mates,flag_done,n)
    for (NODE_T u =0;u<n;u++) {
      //std::cout<<u<<"\n";
      if (available[u] == true) {
        //isInCandP1[u] = false; 
        
         
        //std::cout<<u<<" "<<mates[u]<<std::endl;
        auto vIt = std::next(adjList[u].begin(),mates[u]); 
        //find the current best mate
        for ( ; vIt != adjList[u].end(); vIt++ ) {
          auto v = vIt->v; 
          if(available[v] == true) {
            //mates[u] = std::distance(adjList[u].begin(),vIt); 
            mates[u] = vIt - adjList[u].begin();
            //candItemsP2.push_back(u); 
            flag_done = false;
            break;
          }
        }

        if (vIt == adjList[u].end()) {
          mates[u] = adjList[u].size();
          available[u] = false; 
        }
      } 
      //std::cout<<u<<" "<<mates[u]<<std::endl;
    }
   
    //std::cout<<"Phase 1 timing and work "<<omp_get_wtime() - t1P1<<" "<<candItemsP1.size()<<"\n";
  }
  
  double postProcessEnd = omp_get_wtime();
  //constructing the weight of the matching
  SUM_T totalWeight{};
  NODE_T card{};
  for ( NODE_T u=0;u<n;u++) {
    if (mates[u] != adjList[u].size() ) {
        auto v = adjList[u][mates[u]].v; 
        if (u<v) {
          auto w = adjList[u][mates[u]].weight; 
          //matchedEdges.push_back({u,v,w});
          totalWeight = totalWeight + w;
          card++;
      }
    } 
  }
  return {totalWeight,card,n,0,0,postProcessEnd-postProcessStart};
}
