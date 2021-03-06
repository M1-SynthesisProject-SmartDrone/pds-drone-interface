#include "DroneSender_ThreadClass.h"

#include <loguru/loguru.hpp>
#include "channels/channels.h"

using namespace std;

#define DEBUG_DRONESENDER_THREAD

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
        handleControlCommands();
        handleControlMotors();
    }
    LOG_F(INFO, "End of the drone sender thread");
}


void DroneSender_ThreadClass::handleControlCommands()
{
    blc_channel commandsChannel = pdsChannels::controlCommands;
    // ARM
    if (commandsChannel.chars[0] == 1 && m_drone->motors == UNARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Arm drone");
#endif
        // Only arm if the manual control mode "joystick" is neutral
        m_drone->command_directControl(0.0, 0.0, 0.0, 0.0);
        m_drone->command_arm(1);

    }
    // UNARM
    if (commandsChannel.chars[1] == 1 && m_drone->motors == ARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Disarm drone");
#endif
        m_drone->command_arm(0);
    }
    // Start motors to take-off
    if (commandsChannel.chars[2] == 1 && m_drone->motors == ARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Start drone motors");
#endif
        // Launch the rotors at max speed in order to start rotors
        m_drone->command_directControl(0.0, 0.0, 1000.0, 0.0);
        m_drone->command_directControl(0.0, 0.0, 0.0, 0.0);
    }
}

float DroneSender_ThreadClass::findValue(float input)
{
    if (input != 0.0)
    {
        if (input < -0.5)
        {
            return -DRONE_SPEED;
        }
        else if (input > 0.5)
        {
            return DRONE_SPEED;
        }
    }
    return 0.0;
}

void DroneSender_ThreadClass::handleControlMotors()
{
    if (m_drone->motors == ARM)
    {
        // float xValue = 0.0, yValue = 0.0, zValue = 0.0, rValue = 0.0;
            // Each channel is in "neutral" mode if the channel value is equal to 0.0
            // We have a threshold arround 0 (about 1)
        blc_channel motorsChannel = pdsChannels::controlMotors;

        float x = findValue(motorsChannel.floats[0]);
        float y = findValue(motorsChannel.floats[1]);
        float z = findValue(motorsChannel.floats[2]);
        float r = findValue(motorsChannel.floats[3]);

#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Send command direct control with values (%f, %f, %f, %f)", x, y, z, r);
#endif
        m_drone->command_directControl(x, y, z, r);
    }
}
