#include "DroneSender_ThreadClass.h"

#include <loguru/loguru.hpp>
#include "channels/channels.h"

using namespace std;

DroneSender_ThreadClass::DroneSender_ThreadClass(std::shared_ptr<Drone> drone)
    : Abstract_ThreadClass("drone_sender", 30, 25)
{
    m_drone = drone;
}

DroneSender_ThreadClass::~DroneSender_ThreadClass()
{}

void DroneSender_ThreadClass::run()
{
    initRun();

    while (isRunFlag())
    {
        cout << pdsChannels::altitude.floats[0] << endl;
        usleep(200);
    }
    LOG_F(INFO, "End of the drone sender thread");
}
