//
// Created by quothbonney on 12/16/22.
//

#include "rqs.h"

void RQS::Data::init() {
    v_availableRasterFiles = readDataDir();
    for(const auto& i : v_availableRasterFiles)
        std::cout << i.fname << "\n";
}