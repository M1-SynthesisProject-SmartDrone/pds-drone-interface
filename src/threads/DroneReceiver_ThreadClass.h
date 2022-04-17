#ifndef __DRONERECEIVER_THREADCLASS_H__
#define __DRONERECEIVER_THREADCLASS_H__

#include <memory>

#include "Abstract_ThreadClass.h"
#include "drone/Drone.h"

class DroneReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<Drone> m_drone;
public:
    DroneReceiver_ThreadClass(std::shared_ptr<Drone> drone);
    ~DroneReceiver_ThreadClass();

    void run();
};

#endif // __DRONERECEIVER_THREADCLASS_H__