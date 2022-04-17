
#include <iostream>

#include "loguru/loguru.hpp"
#include "config/config_parser.h"


int main(int argc, char *argv[])
{
    loguru::init(argc, argv);

    ConfigParams params = parseConfig(argc, argv);
    LOG_F(INFO, "Start DroneInterface program");


    LOG_F(WARNING, "got param %s", params.drone.serialPath.c_str());

    LOG_F(INFO, "End of the program DroneInterface");
    return 0;
}
