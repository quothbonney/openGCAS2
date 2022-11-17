#include "rqs/rqs.h"
#include "rqs/sfml_front.h"

using namespace RQS::structures;

int main(int argc, char *argv[]) {
    RQS::RasterQuery& rq = RQS::RasterQuery::get();
    rq.init(llPoint{41.4, -91.4});
    RQS::front::DBVis vis;
    std::vector<llPoint> v{
        llPoint{42.1, -92.4}
    };
    vis.loadPoints(v);
    vis.render();
    return 0;
}
