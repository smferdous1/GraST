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
#include<GraST/Match.h>
#include<omp.h>

#include<iostream>
#include<cassert>

#include<stack>
#include<vector>
#include<algorithm>

#define ASSERT 1

void StreamMatch::GenerateAdjList(NODE_T &n,AdjacencyList &adjacencyList, const float eps) {

  WeightEdgeSim we;
  TrueStreamIn sio(fileName);
  n = sio.getNumberOfVert(); 
  std::vector<VAL_T> potentials(n,0);
  adjacencyList.resize(n);
  
  EDGE_T index = 1;
  while(sio.nextElement(we))
  {
    if(we.u >= we.v || we.weight == 0)
      continue;

    //std::cout<<we.u<<" "<<we.v<<" "<<we.weight<<std::endl;
    
    if(we.weight < (1+eps)*(potentials[we.u] + potentials[we.v]))
     continue; 
    VAL_T wPrime = we.weight - (potentials[we.u] + potentials[we.v]);

    adjacencyList[we.u].push_back({we.v,we.weight});
    adjacencyList[we.v].push_back({we.u,we.weight});
    index++;
    
  
    //std::cout<<we.u<<" "<<we.v<<" "<<we.weight<<std::endl;
    potentials[we.u] += wPrime;
    potentials[we.v] += wPrime;
    //std::cout<<we.u<<" "<<we.v<<" "<<we.weight<<std::endl;
  }

  //std::cout<<"starting post processing"<<std::endl<<std::flush;
  //reverse the vector. So the first element is the highest priority one.
  for(NODE_T i=0;i<n;i++) {
    std::reverse(adjacencyList[i].begin(),adjacencyList[i].end()); 
  }

  

}

MatchOutLite StreamMatch::PMatchGW( const float eps, const std::string method)
{
  WeightEdgeSim we;
  TrueStreamIn sio(fileName);
  NODE_T n = sio.getNumberOfVert(); 
  std::vector<VAL_T> potentials(n,0);
  AdjacencyList adjacencyList(n);
  
  EDGE_T index = 1;
  while(sio.nextElement(we))
  {
    if(we.u >= we.v || we.weight == 0)
      continue;

    //std::cout<<we.u<<" "<<we.v<<" "<<we.weight<<std::endl;
    
    if(we.weight < (1+eps)*(potentials[we.u] + potentials[we.v]))
     continue; 
    VAL_T wPrime = we.weight - (potentials[we.u] + potentials[we.v]);

    adjacencyList[we.u].push_back({we.v,we.weight});
    adjacencyList[we.v].push_back({we.u,we.weight});
    index++;
    
  
    //std::cout<<we.u<<" "<<we.v<<" "<<we.weight<<std::endl;
    potentials[we.u] += wPrime;
    potentials[we.v] += wPrime;
    //std::cout<<we.u<<" "<<we.v<<" "<<we.weight<<std::endl;
  }

  //std::cout<<"starting post processing"<<std::endl<<std::flush;
  //reverse the vector. So the first element is the highest priority one.
  for(NODE_T i=0;i<n;i++) {
    std::reverse(adjacencyList[i].begin(),adjacencyList[i].end()); 
  }
  
  MatchOutLite matchOut;
  if (method == "standard")
    matchOut = ldMatching(n,adjacencyList);
  else if (method == "twophases")
    matchOut = ldMatchingTwoPhases(n,adjacencyList);

  else if (method == "twophasespar")
    matchOut = ldMatchingTwoPhasesPar(n,adjacencyList);
  matchOut.n = n;
  matchOut.m = sio.getNumberOfEdg();
  matchOut.stackSize = index - 1;

  /*
  if(ASSERT == 1)
    assert(isValidMatching(n,matchOut.matchedEdges) == 1);
  */
  return matchOut;
       
}
