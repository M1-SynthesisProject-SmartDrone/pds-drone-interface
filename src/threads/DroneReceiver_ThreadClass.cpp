#include "DroneReceiver_ThreadClass.h"

DroneReceiver_ThreadClass::DroneReceiver_ThreadClass(std::shared_ptr<Drone> drone)
    : Abstract_ThreadClass("drone_receiver", 30, 25)
{
    m_drone = drone;
}

DroneReceiver_ThreadClass::~DroneReceiver_ThreadClass()
{}

void DroneReceiver_ThreadClass::run()
{
    initRun();

    while (isRunFlag())
    {
        onStartLoop();



        onEndLoop();
    }
}
