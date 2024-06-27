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

#include<GraST/Types.h>
#include<vector>


// weighted edge
struct WeightEdgeSim {
  NODE_T u;
  NODE_T v;
  VAL_T weight;

  WeightEdgeSim(){}
  WeightEdgeSim (NODE_T u1, NODE_T v1, VAL_T w1)
    : u(u1)
    , v(v1)
    , weight(w1)
  {}

  bool operator==(const WeightEdgeSim& e) const {
    return (this->u == e.u && this->v == e.v && this->weight == e.weight);
  }
  struct HashFunction {
    std::size_t operator()(const WeightEdgeSim& e) const {
      // Compute individual hash values for first, second and third
      // http://stackoverflow.com/a/1646913/126995
      std::size_t res = 17;
      res = res * 31 + std::hash<NODE_T>()( e.u );
      res = res * 31 + std::hash<NODE_T>()( e.v );
      res = res * 31 + std::hash<VAL_T>()( e.weight );
      return res;
    }
  };
};

struct EndPoint {
  NODE_T v;
  VAL_T weight;
};

struct EndPointInt {
  NODE_T v;
  EDGE_T id;
  VAL_T weight;
};

typedef std::vector<WeightEdgeSim> WeightEdgeSimList;

typedef std::vector<EndPoint> Adjacents;
typedef std::vector<Adjacents> AdjacencyList;

class Graph {
 private:
  NODE_T n;
  EDGE_T m;

 public:
  Graph(){} 
  Graph(NODE_T n1, EDGE_T m1)
    : n(n1)
    , m(m1)
  {}
  Graph(NODE_T n1, EDGE_T m1, const WeightEdgeSimList &eList)
    : n(n1)
    , m(m1)
    , edgeList(eList)
  {}
  NODE_T getNV(){ return n;}
  EDGE_T getNE(){ return m;}

  void setNV( NODE_T n1) { n=n1;}
  void setNE( EDGE_T m1) { m=m1;}
  WeightEdgeSimList edgeList; 
};
