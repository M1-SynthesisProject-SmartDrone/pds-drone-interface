#ifndef __DRONERECEIVER_THREADCLASS_H__
#define __DRONERECEIVER_THREADCLASS_H__

#include <memory>

#include "Abstract_ThreadClass.h"
#include "drone/Drone.h"

class DroneReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    std::shared_ptr<Drone> m_drone;
    bool m_isSensorsOnly;

    void handleMessage(mavlink_message_t &message);

    void handleHeartbeat(mavlink_heartbeat_t &heartbeat);
    void handleAltitude(mavlink_altitude_t &altitude);
    void handleAttitude(mavlink_attitude_t &attitude);
    void handleAck(mavlink_command_ack_t &commandAck);
    void handleLocalPosition(mavlink_local_position_ned_t &localPosition);
    void handleGlobalPosition(mavlink_global_position_int_t &globalPosition);
    void handleRawImu(mavlink_raw_imu_t &rawImu);
    void handleHighresImu(mavlink_highres_imu_t &highresImu);
    void handleBatteryStatus(mavlink_battery_status_t &batteryStatus);
public:
    DroneReceiver_ThreadClass(std::shared_ptr<Drone> drone, bool isSensorsOnly = true);
    ~DroneReceiver_ThreadClass();

    void run();
};

#endif // __DRONERECEIVER_THREADCLASS_H__