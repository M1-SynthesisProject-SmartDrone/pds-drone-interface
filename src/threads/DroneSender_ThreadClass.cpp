#include "DroneSender_ThreadClass.h"

#include <loguru/loguru.hpp>
#include "channels/channels.h"

using namespace std;

// #define DEBUG_DRONESENDER_THREAD

DroneSender_ThreadClass::DroneSender_ThreadClass(std::shared_ptr<Drone> drone, bool isMockMode)
    : Abstract_ThreadClass("drone_sender", 30, 25),
    m_isMockMode(isMockMode)
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
        if (m_isMockMode)
        {
            MOCKhandleControlCommands();
            MOCKhandleControlMotors();
        }
        else
        {
            handleControlCommands();
            handleControlMotors();    
        }
        onEndLoop();
    }
    LOG_F(INFO, "End of the drone sender thread");
}

void DroneSender_ThreadClass::handleControlCommands()
{
    // ARM
    if (pdsChannels::controlCommands.chars[0] == 1 && m_drone->motors == UNARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Arm drone");
#endif
        // Only arm if the manual control mode "joystick" is neutral
        m_drone->command_directControl(0.0, 0.0, 0.0, 0.0);
        m_drone->command_arm(1);
    }
    // UNARM
    if (pdsChannels::controlCommands.chars[1] == 1 && m_drone->motors == ARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Disarm drone");
#endif
        m_drone->command_arm(0);
    }
    // Start motors to take-off
    if (pdsChannels::controlCommands.chars[2] == 1 && m_drone->motors == ARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Start drone motors");
#endif
        // Launch the rotors at max speed in order to start rotors
        // Send multiple times to be sure that the drone started his engines
        m_drone->command_directControl(0.0, 0.0, 1000.0, 0.0);
        m_drone->command_directControl(0.0, 0.0, 1000.0, 0.0);
        m_drone->command_directControl(0.0, 0.0, 1000.0, 0.0);
        m_drone->command_directControl(0.0, 0.0, 1000.0, 0.0);
        m_drone->command_directControl(0.0, 0.0, 0.0, 0.0);
    }
}

float DroneSender_ThreadClass::findValue(float input, float maxSpeed)
{
    if (input != 0.0)
    {
        if (input < -0.5)
        {
            return -maxSpeed;
        }
        else if (input > 0.5)
        {
            return maxSpeed;
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
        float x = findValue(pdsChannels::controlMotors.floats[0], DRONE_SPEED);
        float y = findValue(pdsChannels::controlMotors.floats[1], DRONE_SPEED);
        float z = findValue(pdsChannels::controlMotors.floats[2], 800.0);
        float r = findValue(pdsChannels::controlMotors.floats[3], DRONE_SPEED);

#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Send command direct control with values (%f, %f, %f, %f)", x, y, z, r);
#endif
        m_drone->command_directControl(x, y, z, r);
    }
}

// ==== MOCK MODE ====
void DroneSender_ThreadClass::MOCKhandleControlCommands()
{
    // ARM
    if (pdsChannels::controlCommands.chars[0] == 1 && m_drone->motors == UNARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Arm drone");
#endif
        // Set in arm directly
        pdsChannels::state.chars[0] = 1;
        m_drone->motors = ARM;
    }
    // UNARM
    if (pdsChannels::controlCommands.chars[1] == 1 && m_drone->motors == ARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Disarm drone");
#endif
        pdsChannels::state.chars[0] = 0;
        m_drone->motors = UNARM;
    }
    // Start motors to take-off
    if (pdsChannels::controlCommands.chars[2] == 1 && m_drone->motors == ARM)
    {
#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Start drone motors");
#endif
        pdsChannels::state.chars[0] = 1;
    }
}

void DroneSender_ThreadClass::MOCKhandleControlMotors()
{
    if (m_drone->motors == ARM)
    {
        // float xValue = 0.0, yValue = 0.0, zValue = 0.0, rValue = 0.0;
        // Each channel is in "neutral" mode if the channel value is equal to 0.0
        // We have a threshold arround 0 (about 1)

        int x = findValue(pdsChannels::controlMotors.floats[0], 1);
        int y = findValue(pdsChannels::controlMotors.floats[1], 1);
        int z = findValue(pdsChannels::controlMotors.floats[2], 1);
        int r = findValue(pdsChannels::controlMotors.floats[3], 1);

#ifdef DEBUG_DRONESENDER_THREAD
        LOG_F(INFO, "Send command direct control with values (%d, %d, %d, %d)", x, y, z, r);
#endif
        // We will do as the drone moved, as we probably want feedback
        pdsChannels::globalPosition.ints32[0] += x;
        pdsChannels::globalPosition.ints32[1] += y;
        pdsChannels::globalPosition.ints32[2] += z;
        pdsChannels::globalPosition.ints32[7] += r;
        
        pdsChannels::localPositionNed.floats[0] += x * 0.1f;
        pdsChannels::localPositionNed.floats[1] += y * 0.1f;
        pdsChannels::localPositionNed.floats[2] += z * 0.1f;
    }
}
