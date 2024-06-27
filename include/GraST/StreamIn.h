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

#include<string>
#include<vector>
#include <fstream>
#include <random>

#include<GraST/Types.h>
#include<GraST/Graph.h>




// The class StreamIn implements the streaming input functionality for
// graph stream data.
class StreamIn {
 private:
  std::string fileName;
  EDGE_T batchSize;
  Graph G;
  EDGE_T elemCounter;
  std::string weightType;
  void readEdgeList();
  std::vector<EDGE_T> elemOrder;

  bool is_random = false;
  VAL_T maxW = 1000000.0;
  VAL_T minW = 1.0;
  //random number generator
  std::mt19937 engine;
  std::uniform_real_distribution<VAL_T> dist;

 public:
  StreamIn():elemCounter(0){}
  StreamIn(std::string myFile, VAL_T offset = 0, SIZE_T bSize=1);
  bool nextElement(WeightEdgeSim &);
  inline NODE_T getNumberOfVert() {return G.getNV();}
  inline EDGE_T getNumberOfEdg() {return G.getNE();}
};

//Class TrueStreamIn implements the true streaming capabilities.
class TrueStreamIn {
 private:
  std::string fileName;
  std::ifstream fileRead;
  EDGE_T batchSize;
  EDGE_T elemCounter;
  std::string weightType;
  std::string fType;

  bool is_random = false;
  VAL_T maxW = 1000000.0;
  VAL_T minW = 1.0;

  //Graph related attributes
  NODE_T n;
  EDGE_T nnz;
  EDGE_T m;

  //random number generator
  std::mt19937 engine;
  std::uniform_real_distribution<double> dist;

 public:
  TrueStreamIn():elemCounter(0){}
  TrueStreamIn(std::string myFile, VAL_T offset=0, SIZE_T bSize=1);
  bool nextElement(WeightEdgeSim &,bool=false);
  inline NODE_T getNumberOfVert() {return n;}
  inline EDGE_T getNumberOfEdg() {return m;}
  inline void decEdge() {m--;}
};

//Class FeatureMatrixStream implements the streaming for feature matrix data

//class FeatureMatStream {
//  private:
//    std::string fileName;
//    std::vector<std::vector<EndPoint> > dataset;
//    WeightEdgeSim computeEdge();
//    NODE_T n;
//    NODE_T f;
//    NODE_T nnz;
//    NODE_T curI;
//    NODE_T curJ;
//    
//    void dataLoad();
//  public:
//    FeatureMatStream(std::string myFile);
//    bool nextElement(WeightEdgeSim &, bool=false);
//};

class FeatureMatStream {
  private:
    std::string fileName;
    std::vector<std::vector<EndPoint> > dataset;
    WeightEdgeSim computeEdge(bool isDot=false);
    NODE_T n;
    NODE_T f;
    NODE_T nnz;
    EDGE_T m;

    VAL_T offset;

    NODE_T curI;
    NODE_T curJ;

    void dataLoad();
  public:
    FeatureMatStream(std::string myFile, VAL_T offset=0);
    bool nextElement(WeightEdgeSim &, bool=false);
    inline NODE_T getNumberOfVert() {return n;}
    inline EDGE_T getNumberOfEdg() {return m;}
};
