
#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <signal.h>

#include <loguru/loguru.hpp>

#include "config/config_parser.h"

#include "channels/channels.h"

#include "threads/DroneReceiver_ThreadClass.h"
#include "threads/DroneSender_ThreadClass.h"

using namespace std;

const short DRONE_TIMEOUT_LIMIT = 10;

void handleDrone(bool checkDrone, shared_ptr<Drone> drone, char* serialPath, int serialBaudrate);
void initDrone(shared_ptr<Drone> drone, char* serialPath, int serialBaudrate);
void signalHandler(int number);
void exitProperly(int exitStatus);

int main(int argc, char* argv[])
{
    srand(time(0));
    loguru::init(argc, argv);
    LOG_F(INFO, "Start DroneInterface program");

    // Init sigaction handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGFPE, signalHandler);

    LOG_F(INFO, "Parse config");
    ConfigParams params = parseConfig(argc, argv);

    auto drone = make_shared<Drone>();
    handleDrone(params.drone.checkDrone, drone, params.drone.serialPath.data(), params.drone.baudrate);

    vector<unique_ptr<Abstract_ThreadClass>> threads;
    threads.push_back(make_unique<DroneSender_ThreadClass>(drone));
    threads.push_back(make_unique<DroneReceiver_ThreadClass>(drone));

    LOG_F(INFO, "%ld threads stored", threads.size());
    for (auto& thread : threads)
    {
        thread->start();
    }

    for (auto& thread : threads)
    {
        thread->join();
    }

    LOG_F(INFO, "End of the program DroneInterface");
    return EXIT_SUCCESS;
}

void handleDrone(bool checkDrone, shared_ptr<Drone> drone, char* serialPath, int serialBaudrate)
{
    if (checkDrone) {
        try
        {
            initDrone(drone, serialPath, serialBaudrate);
        }
        catch (const std::exception& e)
        {
            LOG_F(ERROR, "Error init drone. %s. Exit the app", e.what());
            sleep(1);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        LOG_F(WARNING, "No checks done on drone");
    }
}

void initDrone(shared_ptr<Drone> drone, char* serialPath, int serialBaudrate)
{
    LOG_F(INFO, "Try connecting to drone on %s with baudrate %d", serialPath, serialBaudrate);
    drone->open(serialPath, serialBaudrate);
    if (drone->init_communication() != 0)
    {
        stringstream ss;
        ss << "Cannot init communication : " << strerror(errno);
        throw runtime_error(ss.str());
    }

    LOG_F(INFO, "Try init drone parameters");
    if (drone->init_parameters(DRONE_TIMEOUT_LIMIT) != 0)
    {
        stringstream ss;
        ss << "Cannot init parameters : " << strerror(errno);
        throw runtime_error(ss.str());
    }
    // TODO : create messages to handle this in a better way
    // If GPS disabled
    // drone->setMode_manual();
    // If GPS enabled
    drone->setMode_position();
    usleep(1000 * 10); // 10ms
}

void signalHandler(int number)
{
    int exitStatus = 0;
    switch (number)
    {
    case SIGINT:
        LOG_F(WARNING, "SIGINT caught");
        exitProperly(0);
        break;
    case SIGTERM:
        LOG_F(WARNING, "SIGTERM caught");
        exitProperly(1);
        break;
    case SIGFPE:
        LOG_F(WARNING, "SIGFPE caught");
        exitProperly(2);
        break;
    default:
        LOG_F(WARNING, "Unhandled signal caught : %d", number);
        break;
    }
}

void exitProperly(int exitStatus)
{
    LOG_F(INFO, "Clear channels");
    pdsChannels::closeChannels();
    exit(exitStatus);
}