#include "GraST/StreamkDM.h"
#include<omp.h>
#include<utility>
#include<cassert>
#include <array>

Stat StreamkDM::bMatchingAlg(float const eps, bool cc, bool merge){
    Stat stat;
    StreamkMatch skmatch(fileName, k); // Initialize the StreamkMatch solver

    // Solve the k-matching problem
    Graph F; //Subgraph induced by the k-matching
    stat = skmatch.Solve(F.edgeList, eps/2);
    F.setNV(stat.n);
    F.setNE(stat.card);

    double color_t1 = omp_get_wtime();
    // Run Misra-Gries on F to get a \Delta_F + 1 coloring of F
    MisraGries mg(F); // Initialize Misra-Gries solver
    mg.Run(cc);
    double color_t2 = omp_get_wtime();
    assert(mg.getNumColors() <= k+1);
    assert(mg.isValidColoring() == 1);

    double trim_t1 = omp_get_wtime();
    mates = mg.getMates();
    colorStats = mg.getColorStats();
    
    // If there is an additional color class, remove the one with the smallest weight
    if (mates.size() == k+1) {
        auto minColorWeight = std::min_element(colorStats.begin(), colorStats.end());
        COLOR_T minColor = std::distance(colorStats.begin(), minColorWeight);

        if (merge) {
            std::vector<EndPoint> matchMerge(stat.n, {-1,-1});
            COLOR_T mergeColor = minColor-1;
            mergeDJMatch(stat, mates[mergeColor], mates[minColor], matchMerge); 
            SUM_T oldWeight = 0.0, newWeight = 0.0;
            NODE_T oldCard = 0, newCard = 0;
            for (auto &e : mates[mergeColor]) {
                if (e.v != -1) {
                    oldWeight += e.weight;
                    oldCard++;
                }
            }
            oldWeight /= 2.0;
            oldCard /= 2;
            mates[mergeColor] = matchMerge;
            for (auto &e : mates[mergeColor]) {
                if (e.v != -1) {
                    newWeight += e.weight;
                    newCard++;
                }
            }
            newWeight /= 2.0;
            newCard /= 2;
            colorStats[mergeColor].first = newWeight;
            colorStats[mergeColor].second = newCard;
            stat.totalWeight += newWeight - oldWeight;
            stat.card += newCard - oldCard;
        }
        stat.totalWeight -= colorStats[minColor].first;
        stat.card -= colorStats[minColor].second;
        mates.erase(mates.begin() + minColor);
        colorStats.erase(colorStats.begin() + minColor);
    }
    
    if (mates.size() < k) {
        std::vector<std::vector<EndPoint>> addMates(k - mates.size(), std::vector<EndPoint>(stat.n, {-1,-1}));
        std::vector<std::pair<SUM_T, NODE_T>> addColorStats(k - colorStats.size(), std::make_pair(0.0, 0));
        mates.insert(mates.end(), std::begin(addMates), std::end(addMates));  
        colorStats.insert(colorStats.end(), std::begin(addColorStats), std::end(addColorStats));
    }
    double trim_t2 = omp_get_wtime();

    //stat.streamPostProcessTime += color_t2 - color_t1 + trim_t2 - trim_t1;
    stat.colTime += color_t2 - color_t1 + trim_t2 - trim_t1;
    stat.dpTime = 0.0;
    assert(mates.size() == k);
    return stat;
}
