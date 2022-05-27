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
    void handleControlMotors();
    
    // When m_drone is not set, those methods are used instead
    void MOCKhandleControlCommands();
    void MOCKhandleControlMotors();

    float findValue(float input, float maxSpeed);

    bool m_isMockMode;
public:
    DroneSender_ThreadClass(std::shared_ptr<Drone> drone, bool isMockMode = false);
    ~DroneSender_ThreadClass();

    void run();
};

#endif // __DRONESENDER_THREADCLASS_H__