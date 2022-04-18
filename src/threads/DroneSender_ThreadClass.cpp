#include "DroneSender_ThreadClass.h"

DroneSender_ThreadClass::DroneSender_ThreadClass(std::shared_ptr<Drone> drone)
    : Abstract_ThreadClass("drone_receiver", 30, 25)
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
        onStartLoop();

        

        onEndLoop();
    }
}
