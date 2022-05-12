#ifndef __CONFIGPARAMS_H__
#define __CONFIGPARAMS_H__

#include <string>

struct GlobalParams
{
    std::string tmpFolderPath;
    bool isMockMode;
};

struct DroneParams
{
    bool checkDrone;
    std::string serialPath;
    int baudrate;
};

/**
 * This struct contains all data parsed in the config file
 */
struct ConfigParams
{
    GlobalParams global;
    DroneParams drone;

    ConfigParams(GlobalParams global, DroneParams drone)
    {
        this->global = global;
        this->drone = drone;
    }
};

#endif // __CONFIGPARAMS_H__