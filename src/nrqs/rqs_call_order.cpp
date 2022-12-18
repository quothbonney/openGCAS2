//
// Created by quothbonney on 12/17/22.
//

#include "rqs.h"
#include "structs.h"

using namespace RQS::structures;
using namespace RQS;

decltype(Data::callOrder::a_callOrderIndex) Data::callOrder::callOrderInit(const llPoint& llPlanePos) {
    for(int i = -1; i < 2; ++i) {
        for(int j = -1; j < 2; ++j) {
            llPoint probedLocation{llPlanePos.lat - (i * BLOCK_SIZE), llPlanePos.lon + (j * BLOCK_SIZE)};

        }
    }
}