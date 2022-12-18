//
// Created by quothbonney on 12/16/22.
//

#include "rqs.h"
#include "structs.h"
#include <algorithm>

void RQS::Data::init() {
    v_availableRasterFiles = readDataDir();
    for(const auto& i : v_availableRasterFiles) {
        m_rasterMap[i.getLL()] = i;
    }
}