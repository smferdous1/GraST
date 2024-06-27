#ifndef MISRAGRIES_H
#define MISRAGRIES_H

#pragma once

#include "GraST/Graph.h"

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <utility>

class MisraGries {
    private:
        Graph &g;
        int delta;

        std::vector<std::vector<EndPoint>> mates; // mate data structure for each color class
        std::vector<std::pair<SUM_T, NODE_T>> colorStats; // weight and cardinality of each color class
        std::vector<WeightEdgeSim> fan;
        std::vector<std::vector<WeightEdgeSim>> adjList;
        std::unordered_map<WeightEdgeSim, COLOR_T, WeightEdgeSim::HashFunction> color;

        COLOR_T colorEdge(WeightEdgeSim &e, bool cc = true); 
        void maximalFan(NODE_T u, WeightEdgeSim e);
        std::vector<WeightEdgeSim> buildCDPath(NODE_T u, COLOR_T c, COLOR_T d);
        void invertCDPath(COLOR_T c, COLOR_T d, std::vector<WeightEdgeSim> &path);
        void shrinkFan(COLOR_T c, std::vector<WeightEdgeSim> &path, NODE_T u);
        void rotateFan();

        bool isColorFree(NODE_T &u, COLOR_T c) {
            assert(c < delta + 1 && c != UNCOLORED);
            return (mates[c][u].v == -1);
        }
        COLOR_T findFreeColor(NODE_T &u) {
            for (COLOR_T i = 0; i < delta+1; i++) {
                if (isColorFree(u, i))
                    return i;
            }
            return UNCOLORED;
        }
        COLOR_T findCommonColor(NODE_T &u, NODE_T &v) {
            for (COLOR_T i = 0; i < delta+1; i++) {
                if (isColorFree(u, i) && isColorFree(v, i))
                    return i;
            }
            return UNCOLORED;
        }
        void setEdgeColor(WeightEdgeSim &e, COLOR_T c) {
            assert(c < delta + 1);

            color[e] = c;
            mates[c][e.u] = {e.v, e.weight};
            mates[c][e.v] = {e.u, e.weight};
            colorStats[c].first += e.weight;
            colorStats[c].second++;
        }
        void removeEdgeColor(WeightEdgeSim &e) {
            COLOR_T c = color[e];
            if (c != UNCOLORED) {
                color[e] = UNCOLORED;
                if (mates[c][e.u].v == e.v)
                    mates[c][e.u] = {-1, -1};
                if (mates[c][e.v].v == e.u)
                    mates[c][e.v] = {-1, -1};

                colorStats[c].first -= e.weight;
                colorStats[c].second--;
            }
        }

    public:
        MisraGries(Graph &f) : g(f) {
            // Builds the adjaceny list
            adjList.assign(g.getNV(), std::vector<WeightEdgeSim>());
            color.reserve(g.getNE());
            for (auto &e : g.edgeList) {
                color[e] = UNCOLORED; 
                adjList[e.u].emplace_back(e.u, e.v, e.weight);
                adjList[e.v].emplace_back(e.u, e.v, e.weight);
            }

            // Computes delta
            int maxDegree = 0;
            for (int i = 0; i < g.getNV(); i++)
                maxDegree = std::max(maxDegree, (int) adjList[i].size());
            delta = maxDegree;
            mates.assign(delta+1, std::vector<EndPoint>(g.getNV(), {-1,-1}));
            colorStats.assign(delta+1, std::make_pair(0.0, 0));
        }

        void Run(bool cc = true);
        std::unordered_map<WeightEdgeSim, COLOR_T, WeightEdgeSim::HashFunction> getColorMap() {
            return color;
        }
        COLOR_T getEdgeColor(WeightEdgeSim &e) {
            return color[e];
        }
        size_t getNumColors() {
            return delta+1;
        }
        std::vector<std::vector<EndPoint>> getMates() {
            return mates;
        }
        std::vector<std::pair<SUM_T, NODE_T>> getColorStats() {
            return colorStats;
        }
        bool isValidColoring() {
            std::vector<bool> localFreeColors(delta+1, true);
            for (int i = 0; i < g.getNV(); i++) {
                if (adjList[i].size() > 0) {
                    for (auto &e : adjList[i]) {
                        if (color[e] != UNCOLORED) {
                            if (localFreeColors[color[e]])
                                localFreeColors[color[e]] = false;
                            else
                                return false;
                        }
                        else
                            return false;
                    }
                    localFreeColors.assign(delta+1, true);
                }
            }
            return true;
        }
};  

#endif // MISRAGRIES_H