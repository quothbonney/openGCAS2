#include "rqs/rqs.h"
#include "rqs/sfml_front.h"
#include<unistd.h>

using namespace RQS::structures;

int main(int argc, char *argv[]) {
    RQS::RasterQuery& rq = RQS::RasterQuery::get();
    rq.init(llPoint{41.4, -91.2});
    RQS::front::DBVis vis;

    vis.start_thread();
    sleep(1);
    rq.forceOriginTransform(llPoint{40.95, -91.4});
    vis.refresh();
    vis.end_thread();
    std::vector<llPoint> v{
        llPoint{42.1, -92.4}
    };
    vis.loadPoints(v);

    return 0;
}
