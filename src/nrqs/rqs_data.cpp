//
// Created by quothbonney on 12/16/22.
//

#include "rqs.h"
#include "structs.h"
#include <algorithm>

void RQS::Data::init() {
    v_availableRasterFiles = readDataDir();
    for(structures::geoTransformData &i : v_availableRasterFiles) {
        m_rasterMap[i.getLL()] = &i;
        std::cout << i.getLL();
    }
    std::cout << "\n\n";
    std::cout << findInRasterMap(structures::llPoint{34.5, -92.2}).get()->getLL();
}

auto RQS::Data::findInRasterMap(const structures::llPoint llLoc) -> std::shared_ptr<structures::geoTransformData> {
    auto rasterIter = m_rasterMap.upper_bound(llLoc);
    if(rasterIter->second->isLLWithin(llLoc) && rasterIter !=m_rasterMap.begin())
        return std::make_shared<structures::geoTransformData>(*rasterIter->second);
    else {
        // IMPORTANT: CURRENTLY IS ASSUMING RASTERS ARE 1 ARC-SECOND SQUARE!!!
        structures::llPoint baseOrigin{std::ceil(llLoc.lat), std::floor(llLoc.lon)};
        // Creates a ghost raster in the dictionary with almost no overhead
        m_rasterMap[baseOrigin] = new structures::geoTransformData;
        return std::make_shared<structures::geoTransformData>(*m_rasterMap[baseOrigin]);
    }
}