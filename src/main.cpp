#include "nrqs/rqs.h"
//#include "rqs/sfml_front.h"
#include <unistd.h>

#define MOVING

using namespace RQS::structures;

int main(int argc, char *argv[]) {
    RQS::Data& RQSData = RQS::Data::instance();
    RQSData.init();
    return 0;
}
