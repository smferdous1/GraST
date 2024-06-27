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

#include "GraST/StreamEC.h"
#include "GraST/Utility.h"

#include<iostream>
#include<vector>
#include<stdlib.h>

int main(int argC, char *argV[]) {

  if(argC != 2)
  {
    std::cout<<"Usage: "<<argV[0]<<" filename"<<std::endl; 
    exit(1);
  }
  std::string fname(argV[1]);

  srand(341287);
  
  std::vector<EndPoint> edgePointers;

  Stat stat;
  StreamEC sec(fname); 

  sec.ECPD<TrueStreamIn>(edgePointers,stat);
  //Match Stats
  SUM_T totWeight = 0.0;
  NODE_T card=0;

  std::vector<bool> covered(stat.n,false);

  for(NODE_T i=0;i<stat.n;i++) {
    NODE_T u = i; 
    NODE_T v = edgePointers[i].v; 
    VAL_T w = edgePointers[i].weight;

    //std::cout<<u<<" "<<v<<" "<<w<<std::endl;
    if(w>0 && covered[u] == false)
    {
      //std::cout<<u<<" "<<v<<" "<<w<<std::endl;
      totWeight += w;
      card++;
      covered[u] = true;
      if(edgePointers[v].v == u)
        covered[v] = true;
    }
  }
  stat.totalWeight = totWeight;
  stat.card = card;
  std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<stat.totalWeight<<" "<<stat.card<<std::endl;
  return 0;
}
