//
// Created by Jack D.V. Carson on 10/13/2022.
// Copyright (C) GNU LESSER GENERAL PUBLIC LICENSE
//

#include "rqs.h"

using namespace RQS::structures;
using namespace RQS;

///\brief Default constructor with initializer list
rqsDataBlock::rqsDataBlock(int id, int posX, int posY,
                           RasterQuery& rq, nPoint origin,
                           llPoint llOrigin)
                           : m_id(id)
                           , m_origin(origin)
                           , m_llOrigin(llOrigin) {
    if(abs(posY) > 1 || abs(posX) > 1) {
        std::stringstream s;
        s << "Block position " << posX << " " << posY << " lies beyond defined raster bounds (-1, 1), (-1, 1)"
          << " for raster with size 9";
        throw std::invalid_argument(s.str());
    }

    init();
    _res = RasterQuery::get().defineLLRes(m_llOrigin);
    m_rqsDataInfo = &rq.m_dataDirTransform;
    m_rqsCallOrder = &rq.m_rasterCallOrder;

    n_readFromRaster();
    std::cout << "Raster Origin: " << m_origin.x << " " << m_origin.y << " " << m_origin.r << "\n\n\n";
    for(int i = 0; i < m_rqsCallOrder->size(); ++i) {
        if(std::get<1>(m_rqsCallOrder[0][i]) == m_origin.r) { /*std::cout<< "Found Index "<< i << "\n" ; break;*/ }
    }
    //debugWriteBitmap();
}

void rqsDataBlock::readFromRaster() {
    // Possibly GNU compiler bug (???) pointer to vector is a vector?, this appears to work though.
    auto raster = m_rqsDataInfo[0][m_origin.r];


    // Check if there are intersections and read from the raster based on which intersections there are (4 options)
    bool xIntersections, yIntersections;
    xIntersections = (m_origin.x + BLOCK_SIZE > raster.r_xSize);
    yIntersections = (m_origin.y + BLOCK_SIZE > raster.r_ySize);

#ifdef __DEBUG_VERBOSE
    std::cout << "Raster Origin: " << m_origin.x << " " << m_origin.y << " " << m_origin.r << "\n";
    std::cout << (xIntersections ? "XINT " : "");
    std::cout << (yIntersections ? "YINT\n" : " \n");
#endif

    // Align the index with the actual raster of the file in the RasterCallOrder
    int rasterIndexInCallOrder;
    for(int i = 0; i < m_rqsCallOrder->size(); ++i) {
        if(std::get<1>(m_rqsCallOrder[0][i]) == m_origin.r) { rasterIndexInCallOrder = i; break; }
    }

    if(!xIntersections && !yIntersections) { // If no intersections
        for(int row = 0; row < BLOCK_SIZE; ++row) {
            // Create buffer; if the raster is undefined, the buffer is set equal to the value 0,
            // Else, the raster is read normally
            const auto* t = &m_rqsCallOrder[0][rasterIndexInCallOrder];
            if(std::get<0>(*t) == nullptr) {
                _spBlock[row].get()[BLOCK_SIZE] = { 0 };
            } else {
                std::get<0>(*t)->RasterIO(GF_Read,
                            m_origin.x, m_origin.y + row,
                            BLOCK_SIZE, 1,
                            _spBlock[row].get(),
                            BLOCK_SIZE, 1,
                            GDT_Int32,
                            0, 0
                );
            }
        }

    } else if(xIntersections && !yIntersections) {
        int maxX = m_rqsDataInfo[0][m_origin.r].r_xSize;
        int scanX = maxX - m_origin.x;
        const auto* t = &m_rqsCallOrder[0][rasterIndexInCallOrder];
        const auto* t2 = &m_rqsCallOrder[0][rasterIndexInCallOrder + 1]; // Raster to the right
        for(int row = 0; row < BLOCK_SIZE; ++row) {
            if(std::get<0>(*t)== nullptr) {
                _spBlock[row].get()[scanX] = { 0 };
                continue;
            }
            if(std::get<0>(*t2) == nullptr) {
                (_spBlock[row].get() + scanX)[BLOCK_SIZE - scanX] = { 0 };
                continue;
            }
            else {
                std::get<0>(*t)->RasterIO(GF_Read, m_origin.x, m_origin.y + row, scanX, 1,
                                  _spBlock[row].get(),
                                  scanX, 1, GDT_Int32,0, 0
                );
                std::get<0>(*t2)->RasterIO(GF_Read, 0, m_origin.y + row, BLOCK_SIZE - scanX, 1,
                                   _spBlock[row].get() + scanX, // Offset pointer by scansize. Surprisingly easy!
                                   BLOCK_SIZE - scanX, 1, GDT_Int32, 0, 0
                );
            }
        }

    } else if(!xIntersections && yIntersections) {
        int maxY = m_rqsDataInfo[0][m_origin.r].r_ySize;
        int scanY = maxY - m_origin.y;
        const auto* t = &m_rqsCallOrder[0][rasterIndexInCallOrder];
        const auto* t2 = &m_rqsCallOrder[0][rasterIndexInCallOrder + 3]; // Raster below origin

        for(int row = 0; row < scanY; ++row) {
            if(std::get<0>(*t) == nullptr) {
                _spBlock[row].get()[BLOCK_SIZE] = { 0 };
                continue;
            }

            else {
                std::get<0>(*t)->RasterIO(GF_Read,
                                 m_origin.x, m_origin.y + row,
                                 BLOCK_SIZE, 1,
                                 _spBlock[row].get(),
                                 BLOCK_SIZE, 1,
                                 GDT_Int32,
                                 0, 0
                );
            }
        }
        for(int row2 = 0; row2 < BLOCK_SIZE - scanY; ++row2) {
            if(std::get<0>(*t2) == nullptr) {
                _spBlock[scanY + row2].get()[BLOCK_SIZE] = { 0 };
            } else {
                std::get<0>(*t2)->RasterIO(GF_Read,
                                 m_origin.x, 0 + row2,
                                 BLOCK_SIZE, 1,
                                 _spBlock[row2 + scanY].get(),
                                 BLOCK_SIZE, 1,
                                 GDT_Int32,
                                 0, 0
                );
            }
        }

    } else if(xIntersections && yIntersections) {
        int maxX = m_rqsDataInfo[0][m_origin.r].r_xSize;
        int maxY = m_rqsDataInfo[0][m_origin.r].r_ySize;
        const auto* tOrigin = &m_rqsCallOrder[0][rasterIndexInCallOrder];
        const auto* tX = &m_rqsCallOrder[0][rasterIndexInCallOrder + 1];
        const auto* tY = &m_rqsCallOrder[0][rasterIndexInCallOrder + 3];
        const auto* tXY = &m_rqsCallOrder[0][rasterIndexInCallOrder + 4];

        int scanX = maxX - m_origin.x;
        int scanY = maxY - m_origin.y;

        for(int row = 0; row < scanY; ++row) {
            if(std::get<0>(*tOrigin)== nullptr) {
                _spBlock[row].get()[scanX] = { 0 };
            } if(std::get<0>(*tX) == nullptr) {
                (_spBlock[row].get() + scanX)[BLOCK_SIZE - scanX] = { 0 };
            }
            else {
                std::get<0>(*tOrigin)->RasterIO(GF_Read,
                                  m_origin.x, m_origin.y + row,
                                  scanX, 1,
                                  _spBlock[row].get(),
                                  scanX, 1,
                                  GDT_Int32,
                                  0, 0
                );
                std::get<0>(*tX)->RasterIO(GF_Read,
                                   0, m_origin.y + row,
                                   BLOCK_SIZE - scanX, 1,
                                   _spBlock[row].get() + scanX, // Offset pointer by scansize. Surprisingly easy!
                                   BLOCK_SIZE - scanX, 1,
                                   GDT_Int32,
                                   0, 0
                );
            }
        }

        for(int row2 = scanY; row2 < BLOCK_SIZE; ++row2) {
            if(std::get<0>(*tY) == nullptr) {
                _spBlock[scanY + row2].get()[scanX] = { 0 };
            } if(std::get<0>(*tXY)== nullptr) {
                (_spBlock[scanY + row2].get() + scanX)[BLOCK_SIZE - scanX] = { 0 };
            } else {
                std::get<0>(*tY)->RasterIO(GF_Read,
                                        m_origin.x, row2 - scanY,
                                        scanX, 1,
                                        _spBlock[row2].get(),
                                        scanX, 1,
                                        GDT_Int32,
                                        0, 0
                );
                std::get<0>(*tXY)->RasterIO(GF_Read,
                                   0, row2 - scanY,
                                   BLOCK_SIZE - scanX, 1,
                                   _spBlock[row2].get() + scanX, // Offset pointer by scansize. Surprisingly easy!
                                   BLOCK_SIZE - scanX, 1,
                                   GDT_Int32,
                                   0, 0
                );
            }
        }
    }
}

///\brief Initialize memory block of size 1024x1024
void rqsDataBlock::init() {
    const int blockSize = BLOCK_SIZE;
    _spBlock = std::make_unique<_spRow[]>(blockSize);

    for(size_t i = 0; i < blockSize; ++i) {
        _spBlock[i] = std::make_unique<int[]>(blockSize);
    }
#ifdef __DEBUG_VERBOSE
    std::cout << "Allocated " << (sizeof(int) * blockSize*blockSize) / 8 <<
                 " bytes of memory in block with id " << m_id << std::endl;
#endif
}

void rqsDataBlock::debugWriteBitmap() {
    std::ofstream bitmap;
    std::stringstream filename;
    filename << "../etc/output_vis/" << std::to_string(m_id) << "bitmap.pgm";
    bitmap.open(filename.str());
    bitmap << "P2\n" << BLOCK_SIZE << ' ' << BLOCK_SIZE << "\n255\n";
    for(int i = 0; i < BLOCK_SIZE; ++i) {
        for(int j = 0; j < BLOCK_SIZE; ++j) {
            int adj_int = (_spBlock[i][j] / 5) % 255;
            bitmap << adj_int << "  ";
        }
        bitmap << "\n";
    }
    std::stringstream command;
}

void rqsDataBlock::readRasterFromTuple(int rasterIndex,
                                       std::tuple<structures::nPoint, structures::nPoint> nLocs,
                                       structures::nPoint blockIndex) {
    assert(std::get<0>(nLocs).r == std::get<1>(nLocs).r);
    int scanY = std::get<1>(nLocs).y - std::get<0>(nLocs).y;
    int scanX = std::get<1>(nLocs).x - std::get<0>(nLocs).x;

    for(int row = 0; row < scanY; ++row) {
        std::get<0>(m_rqsCallOrder[0][rasterIndex])->RasterIO(GF_Read,
                                                      std::get<0>(nLocs).x,
                                                      std::get<0>(nLocs).y + row,
                                                      scanX, 1,
                                                      _spBlock[row + blockIndex.y].get() + blockIndex.x,
                                                      scanX, 1, GDT_Int32,
                                                      0, 0
                                                      );
    }
}

void rqsDataBlock::n_readFromRaster() {
    // Determining an appropriate negative index with a reference to a nearby raster
    nPoint t;
    if(m_origin.isNullPoint()) {
        nPoint working;
        bool isDefined = false;
        // Iterate through the rasterCallOrder, looking for the raster that most closely to the bottom right of the point
        for (int i = 0; i < 9; ++i) {
            // If the raster doesn't exist, skip it (must reference point outside raster database to raster inside database)
            if(std::get<1>(m_rqsCallOrder[0][i]) == -1) continue;
            // Approximate the index of a raster from the llResolution
            int lat = -1 * (m_rqsDataInfo[0][i].lat_o - m_llOrigin.lat) / std::get<0>(_res);
            int lon = -1 * (m_rqsDataInfo[0][i].lon_o - m_llOrigin.lon) / std::get<1>(_res);
            // If both the lat and lon index are negative (means the raster is to the bottom right of the point)
            // Then define a working raster, but continue iterating through the rest of the rasters to determine
            // If there is a better fit
            if (lat < 0 && lon < 0) {
                if(!isDefined) {
                    working = nPoint{lat, lon, std::get<1>(m_rqsCallOrder[0][i])};
                    isDefined = true;
                } else {
                    lat > working.y && lon > working.x
                        ? working = nPoint{lat, lon, std::get<1>(m_rqsCallOrder[0][i])}
                        : working = working;
                }
            }
        }
        t = working;
        isDefined ? : throw rqsTargetNotFoundInCallOrder{};
    } else {
        t = m_origin;
    }

    nPoint ras = t; // GNU compiler is being weird and making me do this or UB???
    int rasterIndexInCallOrder;
    for(int i = 0; i < m_rqsCallOrder->size(); ++i) {
        if(std::get<1>(m_rqsCallOrder[0][i]) == ras.r) { rasterIndexInCallOrder = i; break; }
    }
    ras.r = rasterIndexInCallOrder;
    bool xIntersections, yIntersections;
    xIntersections = (ras.x + BLOCK_SIZE > m_rqsDataInfo[0][rasterIndexInCallOrder].r_xSize) || (ras.x > -1 * BLOCK_SIZE && ras.x < 0);
    yIntersections = (ras.y + BLOCK_SIZE > m_rqsDataInfo[0][rasterIndexInCallOrder].r_ySize) || (ras.y > -1 * BLOCK_SIZE && ras.y < 0);


    if(!xIntersections && !yIntersections) {
        auto edges = std::make_tuple(ras, nPoint{ras.x + BLOCK_SIZE, ras.y + BLOCK_SIZE, rasterIndexInCallOrder});
        readRasterFromTuple(rasterIndexInCallOrder, edges, nPoint{0, 0});
    } else if(xIntersections && !yIntersections) {
        int yBound = m_rqsDataInfo[0][rasterIndexInCallOrder].r_ySize;
        nPoint p1{ras};
        nPoint p2{0, ras.y, rasterIndexInCallOrder + 1};

        nPoint p5{yBound, ras.y + BLOCK_SIZE, rasterIndexInCallOrder};
        nPoint p6{BLOCK_SIZE - (yBound - ras.x), t.y + BLOCK_SIZE, rasterIndexInCallOrder + 1};
        if(p1.x >= 0 && p1.y >= 0 && p5.x >= 0 && p5.y >= 0) {
            auto tie1 = std::make_tuple(p1, p5);
            readRasterFromTuple(rasterIndexInCallOrder, tie1, nPoint{0, 0, 0});
        }
        if(p2.x >= 0 && p2.y >= 0 && p6.x >= 0 && p6.y >= 0) {
            auto tie2 = std::make_tuple(p2, p6);
            readRasterFromTuple(rasterIndexInCallOrder+1, tie2, nPoint{yBound - p1.x, 0, 0});
        }
    }
    std::cout << t;
}


