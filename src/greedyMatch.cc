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

MatchOut greedyMatching(NODE_T n, EDGE_T m, WeightEdgeSimList &edgeList) {
  
  std::sort(edgeList.begin(),edgeList.end(), [](WeightEdgeSim &a, WeightEdgeSim &b) { 
    return a.weight > b.weight;
  });  

  std::vector<bool> cV(n,false);

  WeightEdgeSimList matchedEdges;
  matchedEdges.clear(); 

  for( auto e:edgeList)
  {
    NODE_T u = e.u;
    NODE_T v = e.v;

    if(cV[u] == false && cV[v] == false)
    {

      cV[u] = true; 
      cV[v] = true; 
      matchedEdges.push_back(e);
    }
  } 

  return {matchedEdges};
}

MatchOut greedykMatching(int k, NODE_T n, EDGE_T m, WeightEdgeSimList &edgeList) {
  std::sort(edgeList.begin(),edgeList.end(), [](WeightEdgeSim &a, WeightEdgeSim &b) { 
    return a.weight > b.weight;
  });  

  std::vector<int> saturation(n, 0);
  WeightEdgeSimList matchedEdges;
  matchedEdges.clear(); 

  for(auto e : edgeList) {
    NODE_T u = e.u;
    NODE_T v = e.v;

    if(saturation[u] < k && saturation[v] < k) {
      saturation[u]++; 
      saturation[v]++; 
      matchedEdges.push_back(e);
    }
  } 
  return {matchedEdges};
}
