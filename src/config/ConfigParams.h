#ifndef __CONFIGPARAMS_H__
#define __CONFIGPARAMS_H__

#include <string>

struct GlobalParams
{
    std::string tmpFolderPath;
    bool isMockMode;
};

struct CameraParams
{
    bool checkCamera;
    int deviceId;
    int framerate;
};

struct DroneParams
{
    bool checkDrone;
    std::string serialPath;
    bool isSensorsOnly;
    int baudrate;
};

/**
 * This struct contains all data parsed in the config file
 */
struct ConfigParams
{
    GlobalParams global;
    CameraParams camera;
    DroneParams drone;

    ConfigParams(GlobalParams global, CameraParams camera, DroneParams drone):
        global(global), camera(camera), drone(drone)
    {}
};

#endif // __CONFIGPARAMS_H__