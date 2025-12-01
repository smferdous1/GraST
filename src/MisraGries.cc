#include "GraST/MisraGries.h"

/**
 * @brief   An implementation of the Misra-Gries (Δ+1) edge coloring algorithm.
 * 
 *          References: [1] https://www.cs.utexas.edu/users/misra/psp.dir/vizing.pdf
 *                      [2] https://en.wikipedia.org/wiki/Misra_%26_Gries_edge_coloring_algorithm
 * 
 * @return  Populates the color map of the graph with a proper (Δ+1) edge coloring.
 */
void MisraGries::Run(bool cc) {
    for (auto &e : g.edgeList) {
        if (color[e] == UNCOLORED)
            colorEdge(e, cc);
    }
}

COLOR_T MisraGries::colorEdge(WeightEdgeSim &e, bool cc) { 
    if (cc) {
        COLOR_T c = findCommonColor(e.u, e.v);
        if (c != UNCOLORED) {
            setEdgeColor(e, c);
            return c;
        }
    }

    // Builds a fan of edges incident on u and starting with e
    maximalFan(e.u, e);

    COLOR_T c = findFreeColor(e.u);     // Color that is free on u
    NODE_T f_k = (e.u == fan.back().u) ? fan.back().v : fan.back().u;
    COLOR_T d = findFreeColor(f_k);     // Color that is free on F[k]

    // A cd_u path must start with a fan edge incident on u with color d
    if (!isColorFree(e.u, d)) {
        std::vector<WeightEdgeSim> path = buildCDPath(e.u, c, d);
        invertCDPath(c, d, path);
        shrinkFan(c, path, e.u);
    }
    
    WeightEdgeSim uw = fan.back();
    rotateFan();
    assert(color[uw] == UNCOLORED);
    setEdgeColor(uw, d);
    return color[e];
}

/**
 *  @brief Builds a maximal fan of vertex u. 
 * 
 *  @param u The vertex to center the fan on. 
 *  @param e The edge to be colored and start the fan with. Note that e must be incident on u and must be uncolored.
 */
void MisraGries::maximalFan(NODE_T u, WeightEdgeSim e) {
    assert(e.u == u || e.v == u);
    assert(color[e] == UNCOLORED);
    fan.clear();
    std::vector<WeightEdgeSim> coloredEdges; // Edges to build the fan with
    coloredEdges.reserve(adjList[u].size());

    for (WeightEdgeSim &a : adjList[u]) {
        if (color[a] != UNCOLORED)
            coloredEdges.push_back(a);
    }
    fan.push_back(e);

    bool maximal;
    std::vector<WeightEdgeSim> unusedEdges;
    unusedEdges.reserve(adjList[u].size());
    while (!maximal) {
        unusedEdges.clear();
        size_t prev = fan.size();
        for (WeightEdgeSim &a : coloredEdges) {
            NODE_T lastNode = (fan.back().u == u) ? fan.back().v : fan.back().u;
            if (isColorFree(lastNode, color[a]))
                fan.push_back(a);
            else 
                unusedEdges.push_back(a);
        }
        maximal = prev == fan.size();
        if (!maximal)
            coloredEdges = unusedEdges;
    }
}

/**
 *  @brief Builds a maximal cd_u path. 
 * 
 *  @param u The vertex to start the cd path on. 
 *  @param c The color that is free on u.
 *  @param d The color that is free on F[k]. Because the the path starts at u and color c is free on u, the first edge in the path must have color d.
 */
std::vector<WeightEdgeSim> MisraGries::buildCDPath(NODE_T u, COLOR_T c, COLOR_T d) {
    std::vector<WeightEdgeSim> path;
    bool maximal = false;
    COLOR_T nextColor = d;
    NODE_T lastNode = u;

    while (!maximal) {
        size_t prev = path.size();
        if (!isColorFree(lastNode, nextColor)) {
            NODE_T i = (lastNode < mates[nextColor][lastNode].v) ? lastNode : mates[nextColor][lastNode].v;
            NODE_T j = (i == lastNode) ? mates[nextColor][lastNode].v : lastNode;
            path.push_back({i, j, mates[nextColor][lastNode].weight});
            lastNode = mates[nextColor][lastNode].v;
        }
        
        nextColor = (nextColor == c) ? d : c;
        maximal = prev == path.size();
    }
    return path;
}

/**
 *  @brief Inverts the colors of a cd path. 
 * 
 *  @param c The color that must be inverted to d.
 *  @param d The color that must be inverted to c.
 *  @param path The cd path to invert.
 */
void MisraGries::invertCDPath(COLOR_T c, COLOR_T d, std::vector<WeightEdgeSim> &path) {
    for (auto &e : path) {
        COLOR_T newColor = (color[e] == c) ? d : c;
        removeEdgeColor(e);
        setEdgeColor(e, newColor);
    }
}

/**
 *  @brief Shrinks the fan such that the last edge in it has the color d free on it.
 * 
 *  @param c The color that must be inverted to d.
 *  @param path The cd_u path after inversion
 *  @param u The starting node of the cd_u path
 */
void MisraGries::shrinkFan(COLOR_T c, std::vector<WeightEdgeSim> &path, NODE_T u) {
    int wIdx = 0;
    for (int i = 0; i < fan.size(); i++) {
        if (color[fan[i]] == c) {
            wIdx = i - 1;
            break;
        }
    }

    NODE_T w = (fan[wIdx].u == u) ? fan[wIdx].v : fan[wIdx].u;
    bool inPath = false;
    for (auto &e : path) {
        if (e.u == w || e.v == w) {
            inPath = true;
            break;
        }
    }
    // If w is in the cd_u path, then there is no need to shrink the fan. Otherwise, shrink the fan 
    // from {(u, v_1), (u, v_2), ..., (u, v_k)} to {(u, v_1), (u, v_2), ..., (u, w)}
    if (!inPath) {
        fan.erase(fan.begin() + wIdx + 1, fan.end());
    }
}

/**
 *  @brief Does a left shift on the colors of edges in the fan. 
 * 
 *  If the fan is {(u, v_1), (u, v_2), ..., (u, v_k)}, then this operation does c(u, v_{i}) ← c(u, v_{i+1}) for all i ∈ [1, k-1] and c(u, v_k) ← UNCOLORED.
 *  This operation leaves the coloring valid, as for each i, c(u, v_{i+1}) was free on c(u, v_{i}) by construction of the fan.
 */
void MisraGries::rotateFan() {
    for (int i = 0; i < fan.size() - 1; i++) {
        removeEdgeColor(fan[i]);
        setEdgeColor(fan[i], color[fan[i+1]]);
    }
    removeEdgeColor(fan.back());
}
