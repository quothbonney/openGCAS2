//
// Created by quothbonney on 12/16/22.
//

#ifndef OPENGCAS_RQS_H
#define OPENGCAS_RQS_H

#define BLOCK_SIZE 1024

#include <vector>
#include <memory>
#include <string>
#include "structs.h"
#include "gdal_priv.h"

namespace RQS {
    class RQSDataBlock;

	struct Data {
        static Data& instance() {
           static Data instance;
           return instance;
        }

        Data(Data const&) = delete;
        Data(Data&&) = delete;


        std::vector<structures::geoTransformData> v_availableRasterFiles;

		struct callOrder {
			std::array<int, 9> a_callOrderIndex;

			std::array<GDALRasterBand*, 9> a_callOrderRawBand;
		};
		
		struct dbInfo {
			std::array<structures::nPoint, 9> a_dbNOrigins;
			std::array<structures::llPoint, 9> a_dbLLOrigins; 
		};

		std::array<std::unique_ptr<RQS::RQSDataBlock>, 9> sp_dbArray;
    private:
        Data() = default;
	};

	class RQSDataBlock {
	private:
		structures::rasterRes _blockResolution;

		typedef std::unique_ptr<int[]> _sp_row;
		std::unique_ptr<_sp_row[]> _sp_block;

		void memory_init();


	};

    auto readDataDir() -> std::vector<structures::geoTransformData>
}

#endif //OPENGCAS_RQS_H
