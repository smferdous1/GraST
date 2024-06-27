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

#pragma once

#include<GraST/StreamIn.h>
#include <GraST/Match.h>
#include<GraST/Utility.h>

class StreamMatch {
 private:
  std::string fileName; 
  bool simStream;
 public:
  StreamMatch(std::string myFile)
    : fileName(myFile)
  {}
  template<class SIn>
  void MatchGW(std::vector<WeightEdgeSim> &, Stat &, float const = 0.001, VAL_T offset=0,bool=false);
  template<class SIn>
  KMatchOut kDMatchGW( Stat &, NODE_T, bool maximal = false,float const = 0.001);
  template<class SIn>
  void MatchFb(std::vector<EndPoint> &, Stat &, float const = 0.001);
  template<class SIn>
  void MatchGW(std::vector<WeightEdgeSim> &, std::vector<EndPoint> &, std::vector<bool> &, Stat &, float = 0.001,bool=false);
  MatchOutLite PMatchGW( const float = 0.001,const std::string = "twophases");
  void GenerateAdjList(NODE_T &, AdjacencyList &, const float = .001);
  //~StreamMatch();
};

