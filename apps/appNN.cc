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


#include<iostream>
#include<vector>

#include "GraST/StreamEC.h"
#include "GraST/Utility.h"

#include<stdlib.h>

int main(int argC, char *argV[]) {
  std::string fname(argV[1]);

  srand(341287);

  std::vector<EndPoint> nnEdge;

  Stat stat;
  StreamEC sec(fname);

  sec.ECNN<TrueStreamIn>(nnEdge, stat);

  // Match Stats
  SUM_T totWeight = 0.0;
  NODE_T card = 0;

  std::vector<bool> covered(stat.n, false);

  for (NODE_T i=0; i < stat.n; i++) {
    NODE_T u = i;
    NODE_T v = nnEdge[i].v;
    VAL_T w = nnEdge[i].weight;


    if (w > 0 && covered[u] == false) {
      //std::cout<<u<<" "<<v<<" "<<w<<std::endl;
      totWeight += w;
      card++;
      covered[u] = true;
      if (nnEdge[v].v == u)
        covered[v] = true;
    }
  }
  stat.totalWeight = totWeight;
  stat.card = card;
  std::cout<<std::fixed<<stat.n<<" "<<stat.m<<" "<<stat.totalWeight<<" "<<stat.card<<std::endl;
  return 0;
}
