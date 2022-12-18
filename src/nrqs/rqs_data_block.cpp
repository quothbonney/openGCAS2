//
// Created by quothbonney on 12/16/22.
//

#include "rqs.h"

using namespace RQS;

void RQSDataBlock::memory_init() {
    _sp_block = std::make_unique<_sp_row[]>(BLOCK_SIZE);
    for(size_t i = 0; i < BLOCK_SIZE; ++i) {
        _sp_block[i] = std::make_unique<int[]>(BLOCK_SIZE);

#ifdef __DEBUG_VERBOSE
        std::cout << "Allocated " << (sizeof(int) * BLOCK_SIZE * BLOCK_SIZE)
                  << " bytes of memory in block with id " << i << std::endl;
#endif
    }
}