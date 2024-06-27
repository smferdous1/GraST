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


#include<GraST/StreamIn.h>

#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <algorithm>

StreamIn::StreamIn(std::string myFile, VAL_T offset, SIZE_T bSize)
  : fileName(myFile)
  , batchSize(bSize)
  , elemCounter(0) {
  readEdgeList();
  //elemOrder.resize(getNumberOfEdg());
  //std::iota(elemOrder.begin(),elemOrder.end(),0);
  //std::shuffle(elemOrder.begin(), elemOrder.end(), std::mt19937{std::random_device{}()});
  }

void StreamIn::readEdgeList() {
  std::ifstream fileRead(fileName.c_str());
  if(fileRead.is_open()==false)
  {
    std::cout << "No file named "<<fileName<<std::endl;
    std::exit(1);
  }

  //Read the mtx header to determine file type 
  std::string header[6];
  fileRead>>header[0]>>header[1]>>header[2]>>header[3]>>header[4];

  weightType = header[3];
  fileRead.seekg(0, std::ios::beg); 

  //Ignore header and comments
  while (fileRead.peek() == '%') fileRead.ignore(2048, '\n');

  NODE_T nrow,ncol;
  EDGE_T nnz;
  fileRead >> nrow >> ncol >> nnz;
 // std::cout<<nnz<<std::endl;
  G.edgeList.reserve(nnz);

  NODE_T u;
  NODE_T v;
  VAL_T weight;
  EDGE_T nEdge = 0;

  engine.seed(341287);
  dist = std::uniform_real_distribution<VAL_T> (minW,maxW);

  if(weightType.compare("pattern") == 0)
    is_random = true;

  G.edgeList.reserve(nnz);
  for(EDGE_T i=0;i<nnz;i++)
  {
    weight = 1.0;
    if(weightType.compare("pattern") != 0)
      fileRead >> v >> u >> weight;
    else
      fileRead >> v >> u;
    v--;
    u--;
    if(is_random)
      weight = dist(engine);

    if(weight < 0) weight = weight*(-1);
    if(u<v && weight>0)
    {
      G.edgeList.push_back({u,v,weight});    
      nEdge++;
    }
  }
  fileRead.close();
  G.setNV(nrow);
  G.setNE(nEdge);
}

bool StreamIn::nextElement(WeightEdgeSim &we) {
  if(elemCounter < getNumberOfEdg())
  {
    we = G.edgeList[elemCounter];
    elemCounter++;
    return true;
  }
  return false;

}

//TrueStreamIn functions

TrueStreamIn::TrueStreamIn(std::string myFile, VAL_T offset, SIZE_T bSize)
  : fileName(myFile)
  , batchSize(bSize)
  , elemCounter(0) 
  , m(0)
  , n(0) 
  , nnz(0) {

  fileRead.open(myFile.c_str(), std::ifstream::in);

  //read the fyle type. 
 
  //mtx 
  fType = myFile.substr(myFile.find_last_of(".") + 1);
  if( fType== "mtx")
  {
    //Read the mtx header to determine file type 
    std::string header[6];
    fileRead>>header[0]>>header[1]>>header[2]>>header[3]>>header[4];

    weightType = header[3];
    fileRead.seekg(0, std::ios::beg); 

    //Ignore header and comments
    while (fileRead.peek() == '%') fileRead.ignore(2048, '\n');

    NODE_T nrow,ncol;
    fileRead >> nrow >> ncol >> nnz;
    n = nrow;
    
    if(weightType.compare("pattern") == 0)
    {
      is_random = true; 
    }
  }
  else if(fType == "kron")
  {
    fileRead >> n >> nnz; 
    is_random = true;
  }
  engine.seed(341287);
  dist = std::uniform_real_distribution<VAL_T> (minW,maxW);

}

bool TrueStreamIn::nextElement(WeightEdgeSim &we,bool rev) {
  if(elemCounter < nnz)
  {
    VAL_T weight = 1;
    NODE_T u,v;
    if(fType == "mtx")
    {
      if(weightType.compare("pattern") != 0)
      {
        if(rev==false) fileRead >> v >> u >> weight;
        else fileRead >> u >> v >> weight;
      }
      else
      {
        if (rev == false) fileRead >> v >> u;
        else fileRead >> u >> v;
      }
    }
    else if(fType == "kron")
    {
       fileRead >> v >> u;
       //std::cout<<u<<" "<<v<<std::endl;
    }

    if(is_random == true)
    {
      //weight = minW + ((maxW-minW)*((VAL_T)rand()/RAND_MAX)); 
      weight = dist(engine);
    }
    elemCounter++;
    //std::cout<<elemCounter<<std::endl;
    v--;
    u--;

    if(weight < 0) weight = weight*(-1);
    we.u = u;
    we.v = v;
    we.weight = weight;

    if(u<v && (weight > 0)) m++;

    return true;
  }
  else if (fileRead.is_open()) {
    fileRead.close();
  }
  return false;

}

/*
void FeatureMatStream::dataLoad() {
  std::ifstream fileread(fileName.c_str());
  if(fileread.is_open()==false)
  {
      std::cout << "No file named "<<fileName<<std::endl;
      std::exit(1);
  }
  //Ignore header and comments
  while (fileread.peek() == '%') fileread.ignore(2048, '\n');

  NODE_T n,f;
  EDGE_T nnz;
  fileread >> n >> f >> nnz; 

  NODE_T u,v;
  VAL_T weight;
  
  dataset.resize(n);
  for(EDGE_T i=0;i<nnz;i++)
  {
    fileread >> u >> v >> weight;
        //fileread >> u >> v;
        //weight = 1.0;
        v--;
        u--;

        dataset[u].push_back({v,weight});
    }

}

WeightEdgeSim FeatureMatStream::computeEdge() {
  VAL_T dist = 0.0;

  auto edgeI= dataset[curI].front(); 
  auto edgeJ= dataset[curJ].front(); 
  while(1) {

  }
}

bool FeatureMatStream::nextElement(WeightEdgeSim &we,bool rev) {
  
  bool status = true;
  while(1) {
    curJ++;
    if(curJ >= f) {
      curI++;
      curJ=0; 
    }
    if(curI >= n){
      status = false;
      break;
    }
     we=computeEdge();
  }
  return status;
} 
*/
FeatureMatStream::FeatureMatStream(std::string myFile, VAL_T _offset) {
  n=0;
  f=0;
  nnz=0;
  m=0;
  curI=0;
  curJ=curI+1;
  fileName = myFile;
  dataLoad();
  //if offset > 0 we are computing distance; so need offset to go to similarity measure
  offset = _offset;

}

void FeatureMatStream::dataLoad() {
  std::ifstream fileread(fileName.c_str());
  if(fileread.is_open()==false)
  {
      std::cout << "No file named "<<fileName<<std::endl;
      std::exit(1);
  }
  //Ignore header and comments
  while (fileread.peek() == '%') fileread.ignore(2048, '\n');

  fileread >> n >> f >> nnz;

  NODE_T u,v;
  VAL_T weight;

  dataset.resize(n);
  for(EDGE_T i=0;i<nnz;i++)
  {
    fileread >> u >> v >> weight;
    //fileread >> u >> v;
    //weight = 1.0;
    v--;
    u--;

    dataset[u].push_back({v,weight});
    std::sort(dataset[u].begin(),dataset[u].end(),
      [](const EndPoint& ep1, const EndPoint& ep2) {
        return ep1.v < ep2.v;
      });
  }

}

WeightEdgeSim FeatureMatStream::computeEdge(bool isDot) {
  VAL_T dist = 0.0;
  VAL_T normFirst = 0.0;
  VAL_T normSecond = 0.0;

  auto &firstN= dataset[curI];
  auto &secondN= dataset[curJ];

  NODE_T firstI =0;
  NODE_T secondI = 0;


  while(firstI < firstN.size() && secondI < secondN.size()) {

    //std::cout<<firstN[firstI].v<<" "<<secondN[secondI].v<<std::endl;

    if ( firstN[firstI].v < secondN[secondI].v) {
      if(isDot) normFirst = normFirst + firstN[firstI].weight*firstN[firstI].weight;
      else dist = dist + firstN[firstI].weight*firstN[firstI].weight;
      firstI++;
    }
    else if (firstN[firstI].v > secondN[secondI].v) {
      if(isDot) normSecond = normSecond + secondN[secondI].weight*secondN[secondI].weight;
      else dist = dist + secondN[secondI].weight*secondN[secondI].weight;
      secondI++;
    }
    else {
      if(isDot) {
        dist = dist+firstN[firstI].weight*secondN[secondI].weight;
        normFirst = normFirst + firstN[firstI].weight*firstN[firstI].weight;
        normSecond = normSecond + secondN[secondI].weight*secondN[secondI].weight;
      }
      else
        dist = dist + firstN[firstI].weight*firstN[firstI].weight+secondN[secondI].weight*secondN[secondI].weight - 2*firstN[firstI].weight*secondN[secondI].weight;
      firstI++;
      secondI++;
    }

  }

  while(firstI <firstN.size()) {
    if(isDot) {
      normFirst = normFirst + firstN[firstI].weight*firstN[firstI].weight;
    }
    else
      dist = dist + firstN[firstI].weight*firstN[firstI].weight;
    firstI++;
  }

  while(secondI <secondN.size()) {
    if(isDot) {
      normSecond = normSecond + secondN[secondI].weight*secondN[secondI].weight;
    }
    else
      dist = dist + secondN[secondI].weight*secondN[secondI].weight;
    secondI++;
  }
  //std::cout<<curI<<" "<<curJ<<" "<<normFirst<<" "<<normSecond<<std::endl;
  if(isDot) {
    //std::cout<<dist<<std::endl;
    dist = dist/(sqrt(normFirst)*sqrt(normSecond));
  }
  else if(dist>0.0) {
    //dist = (VAL_T)1.0/dist;
    dist = offset - dist;
  }

  if (dist == 0.0)
    return {curI,curJ,dist};
  else
    return {curI,curJ,dist};

}

bool FeatureMatStream::nextElement(WeightEdgeSim &we,bool rev) {

  bool status = true;
  while(1) {
    if(curJ >= n) {
      //std::cout<<curI<<std::endl;
      curI++;
      curJ=curI+1;
    }
    if(curI >= n-1){
      status = false;
      break;
    }

    if(offset>0)
      we=computeEdge(false);
    else
      we=computeEdge(true);

    if(we.weight > 0.00) {
      m++;
      curJ++;
      break;
    }
    curJ++;
  }

  return status;
}
