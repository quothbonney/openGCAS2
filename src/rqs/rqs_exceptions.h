#ifndef OPENGCAS_RQS_EXCEPTIONS_H
#define OPENGCAS_RQS_EXCEPTIONS_H

#include <iostream>

struct rqsIndexNotInCallOrder : public std::exception {
    int _rasterIndex;
    explicit rqsIndexNotInCallOrder(int rasterIndex) : _rasterIndex(rasterIndex) {
        std::cerr << "Index " << _rasterIndex << " not found in static RQS::RasterQuery::m_rasterCallOrder";
    };
};

struct rqsTargetNotFoundInCallOrder : public std::exception {
    rqsTargetNotFoundInCallOrder() {
        std::cerr << "Target not found in iteration of static array RQS::RasterQuery::m_rasterCallOrder";
    };
};

#endif // OPENGCAS_RQS_EXCEPTIONS_H