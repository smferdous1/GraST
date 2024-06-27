/*
 * Copyright (C) 2024  Ferdous,S M <ferdous.csebuet@egmail.com>
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
#include "GraST/EdgeCover.h"

#define DEBUG 0

void findNN(NODE_T n, WeightEdgeSimList &edgeList, std::vector<EDGE_T> &mu) {

  mu.resize(n,-1);
  std::vector<VAL_T> temp_mu(n,-1); 

  for( EDGE_T i = 0; i<edgeList.size(); i++ ) {

    auto we = edgeList[i];
     
    if(temp_mu[we.u] == -1 ||(we.weight < temp_mu[we.u]))
    {
        temp_mu[we.u] = we.weight;
        mu[we.u] = i;

    }
    if(temp_mu[we.v] == -1 || (we.weight < temp_mu[we.v]))
    {

        temp_mu[we.v] = we.weight;
        mu[we.v] = i;

    }
  }

}

void computePruneEdges(WeightEdgeSimList &edgeList, WeightEdgeSimList &pruneEdgeList, std::vector<EDGE_T> &mu) {

  pruneEdgeList.clear();
  for( auto e : edgeList ) {
    NODE_T u = e.u;
    NODE_T v = e.v; 
    NODE_T w = e.weight;

    if(w <= 2*edgeList[mu[u]].weight || w <= 2*edgeList[mu[v]].weight) {
      pruneEdgeList.push_back(e); 
    }
  }
}


void primalDualEC(NODE_T n, EDGE_T m, std::vector<NODE_T> &cV, WeightEdgeSimList &edgeList, WeightEdgeSimList &coverEdgeList) {


  //hold the approximate duals
  std::vector<VAL_T> price(n);

  std::vector<NODE_T> dV(n,0);

  calcDegree(edgeList,dV);

  NODE_T currentCovered = 0;

  for(auto v:dV) {
    if(v == 0) currentCovered++; 
  }

  std::vector<EDGE_T> mu;

  findNN(n,edgeList,mu);

  WeightEdgeSimList pruneEdgeList;

  computePruneEdges(edgeList,pruneEdgeList,mu);

  //bool update = true;

  EDGE_T nReducedEdges = pruneEdgeList.size();
  //std::cout<<m<<" "<< nReducedEdges<<std::endl;
  std::vector<short> edgeFlag(nReducedEdges,0);

  coverEdgeList.clear();
  VAL_T totalWeight = 0;

  cV.resize(n,1);
  std::cout<<currentCovered<<std::endl;
  while(currentCovered<n)
  {
    
    //initializing the price
    for(NODE_T i=0;i<n;i++)
    {
      if(cV[i]>0 && dV[i]>0)
      {
        price[i]=std::numeric_limits<VAL_T>::infinity();
      }
    }
    
    //set the price values
    for(EDGE_T i=0;i<nReducedEdges;i++)
    {
      if(edgeFlag[i]==0)
      {
        NODE_T u = pruneEdgeList[i].u;
        NODE_T v = pruneEdgeList[i].v;
        VAL_T w = pruneEdgeList[i].weight;
        
        int cover = 0;
        if(cV[u]>0)
        {
          cover++; 
        }

        if(cV[v]>0)
        {
          cover++;  
        }
        
        if(cover==0) {edgeFlag[i] = 2;continue;};
        if(cV[u]>0) price[u] = std::min(price[u],w/cover);
        if(cV[v]>0) price[v] = std::min(price[v],w/cover);

      }
    }
    
    ///debug
    if(DEBUG==1)
    {
      for(NODE_T i=0;i<n;i++)
        std::cout<<price[i]<<std::endl;
    }
    ///end_debug

    //covering
    //update = false;
    for(EDGE_T i=0;i<nReducedEdges;i++)
    {
      if(edgeFlag[i]==0)
      {
        NODE_T u = pruneEdgeList[i].u;
        NODE_T v = pruneEdgeList[i].v;
        VAL_T w = pruneEdgeList[i].weight;
        
        if(cV[u]>0 && cV[v]>0)
        {
          if(price[u]==w/2.0 && price[v]==w/2.0)
          {
            //add edge
            cV[u]--;
            cV[v]--;
            currentCovered = currentCovered + 2;
            coverEdgeList.push_back(pruneEdgeList[i]);
            totalWeight = totalWeight + w;
            //update = true;
            edgeFlag[i] = 1;


          }
        }

        else if(cV[u]>0)
        {
          if(price[u]==w)
          {
            //add edge
            cV[u]--;
            cV[v]--;
            currentCovered = currentCovered + 1;
            coverEdgeList.push_back(pruneEdgeList[i]);
            totalWeight = totalWeight + w;
            //update = true;
            edgeFlag[i] = 1;


          }
        }

        else if(cV[v]>0)
        {
          if(price[v]==w)
          {
            //add edge
            cV[u]--;
            cV[v]--;
            currentCovered = currentCovered + 1;
            
            coverEdgeList.push_back(pruneEdgeList[i]);

            totalWeight = totalWeight + w;
            //update = true;
            edgeFlag[i] = 1;


          }
        }
      }
      
    }
  }

}
