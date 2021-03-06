#ifndef __DRONESENDER_THREADCLASS_H__
#define __DRONESENDER_THREADCLASS_H__

#include <memory>

#include "Abstract_ThreadClass.h"
#include "drone/Drone.h"

class DroneSender_ThreadClass : public Abstract_ThreadClass
{
private:
    const float DRONE_SPEED = 200.0;

    std::shared_ptr<Drone> m_drone;

    // One method per blc_channel
    void handleControlCommands();

    float findValue(float input);
    void handleControlMotors();
public:
    DroneSender_ThreadClass(std::shared_ptr<Drone> drone);
    ~DroneSender_ThreadClass();

    void run();
};

#endif // __DRONESENDER_THREADCLASS_H__