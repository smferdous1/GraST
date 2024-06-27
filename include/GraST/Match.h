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
#pragma once

#include "GraST/Graph.h"
#include "GraST/Utility.h"

struct MatchOut {
  WeightEdgeSimList matchedEdges;
  NODE_T n;
  EDGE_T m;
  EDGE_T stackSize;
  double postProcessTime;
};

struct MatchOutLite {
  SUM_T totalWeight;
  NODE_T card;
  NODE_T n;
  EDGE_T m;
  EDGE_T stackSize;
  double postProcessTime;
};

using KMatchOut = std::vector<std::vector<EndPoint>>;

MatchOutLite ldMatching(NODE_T n, AdjacencyList const &adjList ); 
MatchOutLite maxMatching(NODE_T n, AdjacencyList const &adjList ); 
MatchOutLite ldMatchingTwoPhases(NODE_T n, AdjacencyList const &adjList ); 

MatchOutLite ldMatchingTwoPhasesPar(NODE_T n, AdjacencyList const &adjList ); 

MatchOut greedyMatching(NODE_T n, EDGE_T m, WeightEdgeSimList &edgeList);
MatchOut greedykMatching(int k, NODE_T n, EDGE_T m, WeightEdgeSimList &edgeList);

std::pair<NODE_T,bool> bestMate( const NODE_T u, std::vector<bool> const &cV, Adjacents const & adj); 

void DPPathOpt(std::vector<NODE_T> &path, std::vector<VAL_T> &weights, std::vector<EndPoint> &match); 

void DPCycleOpt(std::vector<NODE_T> &path, std::vector<VAL_T> &weights, std::vector<EndPoint> &match); 

void mergeDJMatch(Stat & , std::vector<EndPoint> &match1, std::vector<EndPoint> &match2,
    std::vector<EndPoint> &matchOut);
