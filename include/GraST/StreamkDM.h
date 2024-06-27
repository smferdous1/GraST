#ifndef STREAMKDM_H
#define STREAMKDM_H

#pragma once

#include "GraST/Match.h"
#include "GraST/StreamkMatch.h"
#include "GraST/MisraGries.h"
#include <vector>

class StreamkDM {
    private:
        std::string fileName; 
        int k;
        std::vector<std::vector<WeightEdgeSim>> kDM;
        KMatchOut mates;
        std::vector<std::pair<SUM_T, NODE_T>> colorStats;

    public:
        StreamkDM(std::string myFile, int i) : fileName(myFile), k(i) {
           kDM.assign(k, std::vector<WeightEdgeSim>());
        }

    Stat bMatchingAlg(float const eps = 0.001, bool cc = true, bool merge = true);
    std::vector<std::vector<WeightEdgeSim>> getkDM() {
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < mates[i].size(); j++) {
                if (mates[i][j].v != -1 && j < mates[i][j].v)
                    kDM[i].push_back({j, mates[i][j].v, mates[i][j].weight});
            }
        }
        return kDM;
    }
    KMatchOut getkDMMateArray() {
        return mates;
    }
    std::vector<std::pair<SUM_T, NODE_T>> getColorStats() {
        return colorStats;
    }
    void setKVal(int i) {
      k = i; 
    }
};

#endif // STREAMKDM_H
