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
#include "GraST/MisraGries.h"
#include <string>
#include <chrono>

//for  mem usage 
#include <sys/resource.h>


struct Stat {
  NODE_T n{0};
  EDGE_T m{0};
  SUM_T totalWeight{0};
  NODE_T card{0};
  EDGE_T stackSize{0};
  EDGE_T edgeRetained{0};
  VAL_T ub;
  long long mem;

  int simStream{0};
  int dualTight{0};
  double streamReadTime{0};
  double streamReadProcessTime{0};
  double streamProcessTime{0};
  double streamPostProcessTime{0};
  double colTime{0};
  double dpTime{0};
};

/* Edited from https://gist.github.com/mcleary/b0bf4fa88830ff7c882d */
class StopWatch
{
public:
    void start()
    {
        m_StartTime = std::chrono::steady_clock::now();
        m_bRunning = true;
    }

    void stop()
    {
        m_EndTime = std::chrono::steady_clock::now();
        m_bRunning = false;
    }

    double elapsedMilliseconds()
    {
        std::chrono::time_point<std::chrono::steady_clock> endTime;

        if(m_bRunning)
        {
            endTime = std::chrono::steady_clock::now();
        }
        else
        {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }

    double elapsedNanoseconds()
    {
        std::chrono::time_point<std::chrono::steady_clock> endTime;

        if(m_bRunning)
        {
            endTime = std::chrono::steady_clock::now();
        }
        else
        {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - m_StartTime).count();
    }
    double elapsedSeconds()
    {
        return elapsedNanoseconds() / 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
    std::chrono::time_point<std::chrono::steady_clock> m_EndTime;
    bool                                               m_bRunning = false;
};

void permuteInput(std::string infile, std::string outfile); 

void readEdgeList(std::string fileName, NODE_T &n, EDGE_T &m, WeightEdgeSimList &edgeList);
bool isValidMatching(NODE_T n, std::vector<WeightEdgeSim> &matchEdges);
bool isValidkMatching(int k, NODE_T n, std::vector<WeightEdgeSim> &matchedEdges);

void writeOMtx(NODE_T n, AdjacencyList &adjList, std::string outFile);
void readOMtx(NODE_T &, AdjacencyList &adjList, std::string inFile);
void writeDotFile(std::vector<WeightEdgeSim> &edges, std::string outFile);
void writeDotFile(std::vector<WeightEdgeSim> &edges, MisraGries &mg, std::string outFile, const double sat = 0.99, const double val = 0.95);
void writeDotFile(std::vector<std::vector<WeightEdgeSim>> &edges, std::string outFile, const double sat = 0.99, const double val = 0.95);

//long long get_mem_usage();
std::string getLastPartOfFilepath(const std::string& filepath); 
double standardDeviation(const std::vector<NODE_T>& values); 
void calcDegree(WeightEdgeSimList & edgeList, std::vector<NODE_T> &dV); 
