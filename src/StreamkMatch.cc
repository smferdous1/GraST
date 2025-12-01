#include "GraST/StreamkMatch.h"

#include<omp.h>
#include<iostream>
#include<cassert>
#include<stack>
#include<vector>
#include<unordered_map>
#include<utility>
#include<deque>
#include<algorithm>
#include<chrono>

#define ASSERT 1

struct SolutionStackElement {
    WeightEdgeSim edge;
    int r_u; // index in S of the preeceding edge in the vertex stack of u
    int r_v; // index in S of the preeceding edge in the vertex stack of v
    bool ingnore;
};

struct VertexStackElement {
    VAL_T value;
    int sIndex; // index of the edge in the solution stack S

    bool operator<(VertexStackElement const &rhs) const {
        return this->value <= rhs.value;
    }
};

// Helper function to compare the top elements of two stacks
bool stackCompare(std::stack<VertexStackElement, std::vector<VertexStackElement>> &i, std::stack<VertexStackElement, std::vector<VertexStackElement>> &j) { 
    return i.top() < j.top(); 
}

/**
 * @brief   An implementation of the basic semi-streaming max weight b-matching algorithm of Huang and Sellier 
 *          where b(v) = k for all v in V. Uses O(nk * log_{1+eps}(W/eps)) variables in memory, where W is the maximum ratio between 
 *          two non-zero edge weights. 
 * 
 * @param matchedEdges An initially empty vector to store matched edges.
 * @param stat 
 * @param eps   Epsilon parameter for the algorithm. Controls the approximation guarantee of the output. Will be scaled by 1/2 to guarantee
 *              a 1/(2+eps) approximation. 
 * @return  MatchOut: A structure containing matched edges and a bunch of stats. See MatchOut struct in Match.h. 
 *          The solution returned is a 1/(2+eps)-approximate max weight k-matching.
**/
MatchOut StreamkMatch::Solve(std::vector<WeightEdgeSim> &matchedEdges, Stat &stat, float const eps) {
    WeightEdgeSim e;
    TrueStreamIn sio(fileName);
    NODE_T n = sio.getNumberOfVert();
    std::deque<SolutionStackElement> S; // Solution stack as a deque
    //std::vector<std::vector<std::stack<VertexStackElement>>> Q(n, std::vector<std::stack<VertexStackElement>>(k));
    //std::vector<std::vector<std::stack<VertexStackElement, std::vector<VertexStackElement>>>> Q(n, std::vector<std::stack<VertexStackElement, std::vector<VertexStackElement>>>());
    std::vector<std::vector<VertexStackElement>> Q(n, std::vector<VertexStackElement>());

    float eps_scaled = eps/2; // Scale eps to guarantee a 1/(2+eps) approximation

    // [Streaming Phase]
    double t0;
    stat.streamProcessTime = 0.0;
    double tmpReadplusProcess = omp_get_wtime();
    while (sio.nextElement(e)) {
        t0 = omp_get_wtime();
        if (e.u >= e.v || e.weight == 0)
            continue;

        int q_u, q_v;
        VAL_T w_u, w_v;

        if (Q[e.u].size() < k) // May need to allocate another stack in Q[v]
            q_u = -1; // Placeholder index of the vertex stack of u in Q[u]
        else {
            auto result_u = std::min_element(Q[e.u].begin(), Q[e.u].end());
            q_u = std::distance(Q[e.u].begin(), result_u);
        }
        w_u = (q_u == -1) ? 0 : Q[e.u][q_u].value;

        if (Q[e.v].size() < k) // May need to allocate another stack in Q[v]
            q_v = -1; // Placeholder index of the vertex stack of v in Q[v]
        else {
            auto result_v = std::min_element(Q[e.v].begin(), Q[e.v].end());
            q_v = std::distance(Q[e.v].begin(), result_v);
        }
        w_v = (q_v == -1) ? 0 : Q[e.v][q_v].value;

        if (e.weight > (1+eps_scaled) * (w_u + w_v)) {
            VAL_T gain = e.weight - w_u - w_v;
            VAL_T reduced_w_u = w_u + gain;
            VAL_T reduced_w_v = w_v + gain;

            int r_u = (q_u == -1) ? -1 : Q[e.u][q_u].sIndex; // Index in S of the preceding edge in the vertex stack of u
            int r_v = (q_v == -1) ? -1 : Q[e.v][q_v].sIndex; // Index in S of the preceding edge in the vertex stack of v
            S.push_back({e, r_u, r_v, false});
            int solIndex = S.size() - 1;

            if (q_u == -1)
                Q[e.u].push_back({reduced_w_u, solIndex});
            else
                Q[e.u][q_u] = {reduced_w_u, solIndex};

            if (q_v == -1)
                Q[e.v].push_back({reduced_w_v, solIndex});
            else
                Q[e.v][q_v] = {reduced_w_v, solIndex}; 
            //Q[e.u][q_u].value = reduced_w_u;
            //Q[e.u][q_u].sIndex = solIndex;
            //Q[e.v][q_v].value = reduced_w_v;
            //Q[e.v][q_v].sIndex = solIndex;
        }
        stat.streamProcessTime += (omp_get_wtime() - t0);
    }
    stat.streamReadTime = omp_get_wtime() - tmpReadplusProcess - stat.streamProcessTime;
    stat.stackSize = S.size();
    stat.n = sio.getNumberOfVert();
    stat.m = sio.getNumberOfEdg();

    // [Post-Processing Phase]
    EDGE_T stackSize = S.size();
    double t1 = omp_get_wtime();
    // Unwind ths solution stack S
    while (!S.empty()) { 
        SolutionStackElement e_top = S.back();
        if (!e_top.ingnore) {
            matchedEdges.push_back(e_top.edge);
            e_top.ingnore = true;
            
            // Ignore preceding edges in the vertex stacks of u and v
            int prev_u = e_top.r_u;
            int prev_v = e_top.r_v;
            while (prev_u >= 0) {
                S[prev_u].ingnore = true;
                prev_u = (e_top.edge.u == S[prev_u].edge.u) ? S[prev_u].r_u : S[prev_u].r_v;
            }
            while (prev_v >= 0) {
                S[prev_v].ingnore = true;
                prev_v = (e_top.edge.v == S[prev_v].edge.u) ? S[prev_v].r_u : S[prev_v].r_v;
            }
        }
        S.pop_back();
    }

    double t2 = omp_get_wtime();
    stat.streamPostProcessTime = t2-t1;
    if (ASSERT == 1)
        assert(isValidkMatching(k, n, matchedEdges) == 1);
    return {matchedEdges, sio.getNumberOfVert(), sio.getNumberOfEdg(), stackSize, t2-t1};
}

/**
 * @brief   An implementation of the basic semi-streaming max weight b-matching algorithm of Huang and Sellier 
 *          where b(v) = k for all v in V. Uses O(nk * log_{1+eps}(W/eps)) variables in memory, where W is the maximum ratio between 
 *          two non-zero edge weights. 
 * 
 * @param stat 
 * @param eps   Epsilon parameter for the algorithm. Controls the approximation guarantee of the output. Will be scaled by 1/2 to guarantee
 *              a 1/(2+eps) approximation. 
 * @return  WeightEdgeSimList: A vector containing matched edges
 *          The solution returned is a 1/(2+eps)-approximate max weight k-matching.
**/
Stat StreamkMatch::Solve(std::vector<WeightEdgeSim> &matchedEdges, float const eps) {
    Stat stat;
    
    WeightEdgeSim e;
    TrueStreamIn sio(fileName);
    NODE_T n = sio.getNumberOfVert();
    std::deque<SolutionStackElement> S; // Solution stack as a deque
    std::vector<std::vector<VertexStackElement>> Q(n, std::vector<VertexStackElement>());

    float eps_scaled = eps/2; // Scale eps to guarantee a 1/(2+eps) approximation

    // [Streaming Phase]
    double t0;
    stat.streamProcessTime = 0.0;
    double tmpReadplusProcess = omp_get_wtime();
    while (sio.nextElement(e)) {
        t0 = omp_get_wtime();
        if (e.u >= e.v || e.weight == 0)
            continue;

        int q_u, q_v;
        VAL_T w_u, w_v;

        if (Q[e.u].size() < k) { // May need to allocate another stack in Q[v]
            q_u = -1; // Placeholder index of the vertex stack of u in Q[u]
        }
        else {
            auto result_u = std::min_element(Q[e.u].begin(), Q[e.u].end());
            q_u = std::distance(Q[e.u].begin(), result_u);
        }
        w_u = (q_u == -1) ? 0 : Q[e.u][q_u].value;

        if (Q[e.v].size() < k) { // May need to allocate another stack in Q[v]
            q_v = -1; // Placeholder index of the vertex stack of v in Q[v]
        }
        else {
            auto result_v = std::min_element(Q[e.v].begin(), Q[e.v].end());
            q_v = std::distance(Q[e.v].begin(), result_v);
        }
        w_v = (q_v == -1) ? 0 : Q[e.v][q_v].value;

        if (e.weight > (1+eps_scaled) * (w_u + w_v)) {
            VAL_T gain = e.weight - w_u - w_v;
            VAL_T reduced_w_u = w_u + gain;
            VAL_T reduced_w_v = w_v + gain;

            int r_u = (q_u == -1) ? -1 : Q[e.u][q_u].sIndex; // Index in S of the preceding edge in the vertex stack of u
            int r_v = (q_v == -1) ? -1 : Q[e.v][q_v].sIndex; // Index in S of the preceding edge in the vertex stack of v
            S.push_back({e, r_u, r_v, false});
            int solIndex = S.size() - 1;

            if (q_u == -1) {
                Q[e.u].push_back({reduced_w_u, solIndex});
            }
            else {
                Q[e.u][q_u].value = reduced_w_u;
                Q[e.u][q_u].sIndex = solIndex;
            }
            if (q_v == -1) {
                Q[e.v].push_back({reduced_w_v, solIndex});
            }
            else {
                Q[e.v][q_v].value = reduced_w_v;
                Q[e.v][q_v].sIndex = solIndex;
            }
        }
        stat.streamProcessTime += (omp_get_wtime() - t0);
    }
    stat.streamReadTime = omp_get_wtime() - tmpReadplusProcess - stat.streamProcessTime;
    stat.stackSize = S.size();
    stat.edgeRetained = S.size();
    stat.n = sio.getNumberOfVert();
    stat.m = sio.getNumberOfEdg();

    // [Post-Processing Phase]
    double t1 = omp_get_wtime();
    // Unwind ths solution stack S
    while (!S.empty()) { 
        SolutionStackElement e_top = S.back();
        if (!e_top.ingnore) {
            matchedEdges.push_back(e_top.edge);
            e_top.ingnore = true;
            
            // Ignore preceding edges in the vertex stacks of u and v
            int prev_u = e_top.r_u;
            int prev_v = e_top.r_v;
            while (prev_u >= 0) {
                S[prev_u].ingnore = true;
                prev_u = (e_top.edge.u == S[prev_u].edge.u) ? S[prev_u].r_u : S[prev_u].r_v;
            }
            while (prev_v >= 0) {
                S[prev_v].ingnore = true;
                prev_v = (e_top.edge.v == S[prev_v].edge.u) ? S[prev_v].r_u : S[prev_v].r_v;
            }
        }
        S.pop_back();
    }

    double t2 = omp_get_wtime();
    stat.streamPostProcessTime = t2-t1;
    if (ASSERT == 1)
        assert(isValidkMatching(k, n, matchedEdges) == 1);
    
    SUM_T weight = 0.0;
    for (auto &e : matchedEdges)
        weight += e.weight;
    stat.totalWeight = weight;
    stat.card = matchedEdges.size();
    return stat;
}

/**
 * TODO
 * @brief An alternate version of the semi-streaming max weight b-matching algorithm of Huang and Sellier
 *        that uses stack evictions to remove space complexity on W. Uses O(nk * log_{1+eps}(1/eps)) variables in memory
 * 
 * @param matchedEdges  An initially empty vector to store matched edges.
 * @param stat 
 * @param eps   Epsilon parameter for the algorithm. Controls the approximation guarantee of the output. Will be scaled by 1/12 to
 *              guarantee a 1/(2+eps) approximation.
 * @return MatchOut: A structure containing matched edges and a bunch of stats. See MatchOut struct in Match.h. 
 *         The solution returned is a 1/(2+eps)-approximate max weight k-matching.
**/
MatchOut StreamkMatch::SolveAlt(std::vector<WeightEdgeSim> &matchedEdges, Stat &stat, float const eps) {
    WeightEdgeSim e;
    TrueStreamIn sio(fileName);
    NODE_T n = sio.getNumberOfVert();
    std::deque<SolutionStackElement> S; // Solution stack as a deque
    //std::vector<std::vector<std::stack<VertexStackElement>>> Q(n, std::vector<std::stack<VertexStackElement>>(k));
    std::vector<std::vector<std::stack<VertexStackElement, std::vector<VertexStackElement>>>> Q(n, std::vector<std::stack<VertexStackElement, std::vector<VertexStackElement>>>());
    //std::vector<std::vector<VertexStackElement>> Q(n, std::vector<VertexStackElement>());

    std::cout << "Running Alternate Version with evictions active" << std::endl;
    float eps_scaled = eps/12; // Scale eps to guarantee a 1/(2+eps) approximation

    // [Streaming Phase]
    double t0;
    stat.streamProcessTime = 0.0;
    double tmpReadplusProcess = omp_get_wtime();
    while (sio.nextElement(e)) {
        t0 = omp_get_wtime();
        if (e.u >= e.v || e.weight == 0)
            continue;

        int q_u, q_v;
        VAL_T w_u, w_v;

        if (Q[e.u].size() < k) { // May need to allocate another stack in Q[v]
            q_u = -1; // Placeholder index of the vertex stack of u in Q[u]
        }
        else {
            auto result_u = std::min_element(Q[e.u].begin(), Q[e.u].end(), stackCompare);
            q_u = std::distance(Q[e.u].begin(), result_u);
        }
        w_u = (q_u == -1) ? 0 : Q[e.u][q_u].top().value;

        if (Q[e.v].size() < k) { // May need to allocate another stack in Q[v]
            q_v = -1; // Placeholder index of the vertex stack of v in Q[v]
        }
        else {
            auto result_v = std::min_element(Q[e.v].begin(), Q[e.v].end(), stackCompare);
            q_v = std::distance(Q[e.v].begin(), result_v);
        }
        w_v = (q_v == -1) ? 0 : Q[e.v][q_v].top().value;

        if (e.weight > (1+eps_scaled) * (w_u + w_v)) {
            VAL_T gain = e.weight - w_u - w_v;
            VAL_T reduced_w_u = w_u + gain;
            VAL_T reduced_w_v = w_v + gain;

            int r_u = (q_u == -1) ? -1 : Q[e.u][q_u].top().sIndex; // Index in S of the preceding edge in the vertex stack of u
            int r_v = (q_v == -1) ? -1 : Q[e.v][q_v].top().sIndex; // Index in S of the preceding edge in the vertex stack of v
            S.push_back({e, r_u, r_v, false});
            int solIndex = S.size() - 1;

            if (q_u == -1) {
                Q[e.u].push_back(std::stack<VertexStackElement, std::vector<VertexStackElement>>());
                q_u = Q[e.u].size() - 1;
            }
            if (q_v == -1) {
                Q[e.v].push_back(std::stack<VertexStackElement, std::vector<VertexStackElement>>());
                q_v = Q[e.v].size() - 1;
            }
            Q[e.u][q_u].push({reduced_w_u, solIndex});
            Q[e.v][q_v].push({reduced_w_v, solIndex});
        }
        stat.streamProcessTime += (omp_get_wtime() - t0);
    }
    stat.streamReadTime = omp_get_wtime() - tmpReadplusProcess - stat.streamProcessTime;
    stat.stackSize = S.size();
    stat.n = sio.getNumberOfVert();
    stat.m = sio.getNumberOfEdg();

    // [Post-Processing Phase]
    EDGE_T stackSize = S.size();
    double t1 = omp_get_wtime();
    // Unwind ths solution stack S
    while (!S.empty()) { 
        SolutionStackElement e_top = S.back();
        if (!e_top.ingnore) {
            matchedEdges.push_back(e_top.edge);
            e_top.ingnore = true;
            
            // Ignore preceding edges in the vertex stacks of u and v
            int prev_u = e_top.r_u;
            int prev_v = e_top.r_v;
            while (prev_u >= 0) {
                S[prev_u].ingnore = true;
                prev_u = (e_top.edge.u == S[prev_u].edge.u) ? S[prev_u].r_u : S[prev_u].r_v;
            }
            while (prev_v >= 0) {
                S[prev_v].ingnore = true;
                prev_v = (e_top.edge.v == S[prev_v].edge.u) ? S[prev_v].r_u : S[prev_v].r_v;
            }
        }
        S.pop_back();
    }

    double t2 = omp_get_wtime();
    stat.streamPostProcessTime = t2-t1;
    if (ASSERT == 1)
        assert(isValidkMatching(k, n, matchedEdges) == 1);
    return {matchedEdges, sio.getNumberOfVert(), sio.getNumberOfEdg(), stackSize, t2-t1};
}







/*
std::cout << "Pushed e = (" << e.weight << ", " << e.u << ", " << e.v << "), r_u = " << r_u << ", r_v = " << r_v << " to S" << std::endl;
std::cout << "w_u: "<< w_u <<", q_u: "<< q_u;
if (Q[e.u][q_u].empty())
    std::cout << ", Q[" << e.u << "][" << q_u << "] is empty" << std::endl;
else
    std::cout << ", Q[" << e.u << "][" << q_u << "] = " << Q[e.u][q_u].top().value << std::endl;
std::cout << "w_v: "<< w_v <<", q_v: "<< q_v;
if (Q[e.v][q_v].empty())
    std::cout << ", Q[" << e.v << "][" << q_v << "] is empty" << std::endl;
else
    std::cout << ", Q[" << e.v << "][" << q_v << "] = " << Q[e.v][q_v].top().value << std::endl;
std::cout << "Pushed " << reduced_w_u << " to Q[" << e.u << "][" << q_u << "]" << std::endl;
std::cout << "Pushed " << reduced_w_v << " to Q[" << e.v << "][" << q_v << "]" << std::endl << std::endl;
*/
           
/*
for (int i = 0; i < n; i++) {
    std::cout << "----------------------" << std::endl;
    for (int j = 0; j < k; j++) {
        std::stack<VertexStackElement> s = Q[i][j];

        std::cout << "Q[" << i << "][" << j << "]" << std::endl;
        while (!s.empty()) {
            WeightEdgeSim f = S[s.top().sIndex].edge;
            std::cout << "e = (" << f.weight << ", " << f.u << ", " << f.v << ")" << std::endl;
            s.pop();
        }
    }
    std::cout << "----------------------" << std::endl;
}
*/
