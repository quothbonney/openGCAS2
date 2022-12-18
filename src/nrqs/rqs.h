//
// Created by quothbonney on 12/16/22.
//

#ifndef OPENGCAS_RQS_H
#define OPENGCAS_RQS_H

#define BLOCK_SIZE 1024

#include <vector>
#include <memory>
#include <string>
#include "map"
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
        std::map<structures::llPoint, structures::geoTransformData*> m_rasterMap;

		struct callOrder {
			std::array<int, 9> a_callOrderIndex;

			std::array<GDALRasterBand*, 9> a_callOrderRawBand;

            auto callOrderInit(const structures::llPoint& llPlanePos) -> std::array<int, 9>;
		};
		
		struct dbInfo {
			std::array<structures::nPoint, 9> a_dbNOrigins;
			std::array<structures::llPoint, 9> a_dbLLOrigins; 
		};

		std::array<std::unique_ptr<RQS::RQSDataBlock>, 9> sp_dbArray;

        void init();

        auto findInRasterMap(const structures::llPoint llLoc) -> std::shared_ptr<structures::geoTransformData>;

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
    /**
     * @brief define m_dataDirTransform vector attribute with
     * geoTransForm data from data/ directory
     */
    auto readDataDir() -> std::vector<structures::geoTransformData>;

    auto discreteIndex(const structures::llPoint& llLoc) -> structures::nPoint;

    auto getClosest(const structures::llPoint& llLoc, const std::vector<structures::geoTransformData>& d_availableRasterFiles) -> std::shared_ptr<structures::geoTransformData>;
}

#endif //OPENGCAS_RQS_H
