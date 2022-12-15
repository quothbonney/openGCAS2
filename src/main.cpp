#include "rqs/rqs.h"
#include "rqs/sfml_front.h"
#include <unistd.h>

using namespace RQS::structures;

int main(int argc, char *argv[]) {
    RQS::RasterQuery& rq = RQS::RasterQuery::get();
    llPoint origin{41.95, -90.85};
    rq.init(origin);
    RQS::front::DBVis vis;
    //vis.render();

    vis.start_thread();
    sleep(1);
    for(double i = 0; ; i += 0.05) {
        rq.forceOriginTransform(llPoint{origin.lat + i, origin.lon + i});
        vis.refresh();
        sleep(1);
    }

    vis.end_thread();
    std::cout << "Thread";
    std::vector<llPoint> v{
        llPoint{42.1, -92.4}
    };
    vis.loadPoints(v);

    return 0;
}
