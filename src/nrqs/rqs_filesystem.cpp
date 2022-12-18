//
// Created by quothbonney on 12/16/22.
//

#include "rqs.h"

auto RQS::readDataDir() -> std::vector<structures::geoTransformData> {
    GDALAllRegister();
    // Assumes that executable is in the bin directory!!
    std::string rasterDataDir = "../data";
    int rasterIndex = 0;
    std::vector<structures::geoTransformData> geoTransformVector;

    for(const auto& datafile : std::experimental::filesystem::directory_iterator(rasterDataDir)) {
        std::string s_filename= datafile.path().string();
        // Get extension for string filename
        std::string extension = s_filename.substr(s_filename.find_last_of('.') + 1);
        // If the extension is correct try to read it
        try {
            if (extension == "tif" || extension == "tiff") {
                auto e_dataset = (GDALDataset *) GDALOpen(s_filename.c_str(), GA_ReadOnly);
                double GDALTransform[6];
                e_dataset->GetGeoTransform(GDALTransform);
                geoTransformVector.push_back(structures::geoTransformData{
                        rasterIndex,
                        s_filename,
                        GDALTransform[0],
                        GDALTransform[1],
                        GDALTransform[3],
                        GDALTransform[5],
                        e_dataset->GetRasterXSize(),
                        e_dataset->GetRasterYSize(),
                });
                rasterIndex++;
                GDALClose(e_dataset);
            }
        } catch(...) { ;} // Catch any runtime errors GDAL might throw regarding the contents of a file and skip it
    }

    auto sortRasters = [](const structures::geoTransformData& rhs, const structures::geoTransformData& lhs) -> bool {
        return structures::llPoint{lhs.lat_o, lhs.lon_o} > structures::llPoint{rhs.lat_o, rhs.lon_o};
    };

    std::sort(geoTransformVector.begin(), geoTransformVector.end(), sortRasters);

    return geoTransformVector;
}