#ifndef STREAMKMATCH_H
#define STREAMKMATCH_H

#pragma once

#include "GraST/StreamIn.h"
#include "GraST/Match.h"
#include "GraST/Utility.h"

class StreamkMatch {
  private:
    std::string fileName; 
    int k;
  public:
    StreamkMatch(std::string myFile, int i)
      : fileName(myFile), k(i)
    {}

  MatchOut Solve(std::vector<WeightEdgeSim> &, Stat &, float const = 0.001);
  Stat Solve(std::vector<WeightEdgeSim> &, float const eps = 0.001);

  //TODO: Implement alternate version with stack evictions
  MatchOut SolveAlt(std::vector<WeightEdgeSim> &, Stat &, float const = 0.001); 


  //void Solve(std::vector<WeightEdgeSim> &, std::vector<EndPoint> &, std::vector<bool> &, Stat &, float = 0.001);
  //void GenerateAdjList(NODE_T &, AdjacencyList &, const float = .001);
  //~StreamkMatch();
};

#endif // STREAMKMATCH_H