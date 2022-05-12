#include "config_parser.h"

#include <iostream>
#include <filesystem>
#include <libconfig.h++>

using namespace std;
namespace fs = std::filesystem;

/**
 * Find the config file path from the executable path
 */
fs::path getConfigPath(char *argv[])
{
    // argv[0] contains the executable called
    fs::path exePath(argv[0]);
    fs::path exeDirectoryPath = exePath.parent_path();
    fs::path configPath(exeDirectoryPath.string() + "/config.cfg");
    if (!fs::exists(configPath))
    {
        throw runtime_error("Config file not found at path : " + configPath.string());
    }
    return configPath;
}

ConfigParams parseConfig(int argc, char *argv[])
{
    fs::path configPath = getConfigPath(argv);

    libconfig::Config config;
    config.readFile(configPath.c_str());

    const libconfig::Setting& root = config.getRoot();

    const auto& globalParams = GlobalParams{
        root["tmp_folder"],
        root["is_mock_mode"]
    };

    const auto& cameraSettings = root["camera"];
    const auto cameraParams = CameraParams{
        cameraSettings["check_camera"],
        cameraSettings["device_id"],
        cameraSettings["framerate"]
    };
    
    const auto& droneSettings = root["drone"];
    const auto droneParams = DroneParams{
        droneSettings["check_drone"],
        droneSettings["serial_path"],
        droneSettings["baudrate"]
    };

    return ConfigParams(globalParams, cameraParams, droneParams);
}