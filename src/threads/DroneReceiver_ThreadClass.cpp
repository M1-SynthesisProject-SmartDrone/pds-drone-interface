#include "DroneReceiver_ThreadClass.h"

#include <sstream>

#include <loguru/loguru.hpp>
#include "channels/channels.h"

#define DRONERECEIVER_THREADCLASS_DEBUG

using namespace std;

DroneReceiver_ThreadClass::DroneReceiver_ThreadClass(shared_ptr<Drone> drone)
    : Abstract_ThreadClass("drone_receiver", 30, 25)
{
    m_drone = drone;
}

DroneReceiver_ThreadClass::~DroneReceiver_ThreadClass()
{}

void DroneReceiver_ThreadClass::run()
{
    initRun();
    pdsChannels::altitude.floats[0] += 0.0;

    while (isRunFlag())
    {
        // onStartLoop();

        mavlink_message_t message;
        if (m_drone->read_message(message))
        {
            handleMessage(message);
        }
        else
        {
            LOG_F(ERROR, "Cannot read message : %s", strerror(errno));
        }

        pdsChannels::altitude.floats[0] += 0.000001;
        usleep(20);

        // onEndLoop();
    }
    LOG_F(INFO, "End of the drone receiver thread");
}

void DroneReceiver_ThreadClass::handleMessage(mavlink_message_t& message)
{
    switch (message.msgid)
    {
    case MAVLINK_MSG_ID_HEARTBEAT:
    {
        mavlink_heartbeat_t heartbeat;
        mavlink_msg_heartbeat_decode(&message, &heartbeat);
        handleHeartbeat(heartbeat);
        break;
    }
    case MAVLINK_MSG_ID_ALTITUDE:
    {
        mavlink_altitude_t altitude;
        mavlink_msg_altitude_decode(&message, &altitude);
        handleAltitude(altitude);
        break;
    }
    case MAVLINK_MSG_ID_ATTITUDE:
    {
        mavlink_attitude_t attitude;
        mavlink_msg_attitude_decode(&message, &attitude);
        handleAttitude(attitude);
        break;
    }
    case MAVLINK_MSG_ID_COMMAND_ACK:
    {
        mavlink_command_ack_t commandAck;
        mavlink_msg_command_ack_decode(&message, &commandAck);
        handleAck(commandAck);
        break;
    }
    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
    {
        mavlink_global_position_int_t global_position_int;
        mavlink_msg_global_position_int_decode(&message, &global_position_int);
        handleGlobalPosition(global_position_int);
        break;
    }
    case MAVLINK_MSG_ID_SCALED_IMU:
    {
        mavlink_raw_imu_t scaled_imu;
        mavlink_msg_raw_imu_decode(&message, &scaled_imu);
        handleRawImu(scaled_imu);
        break;
    }
    case MAVLINK_MSG_ID_HIGHRES_IMU:
    {
        mavlink_highres_imu_t highres_imu;
        mavlink_msg_highres_imu_decode(&message, &highres_imu);
        handleHighresImu(highres_imu);
        break;
    }
    case MAVLINK_MSG_ID_BATTERY_STATUS:
    {
        mavlink_battery_status_t battery_status;
        mavlink_msg_battery_status_decode(&message, &battery_status);
        handleBatteryStatus(battery_status);
        break;
    }
    default:
        LOG_F(WARNING, "Unhandled message id : %d", message.msgid);
        break;
    }
}

// This is where ar disarm state pass through blc_channels
void DroneReceiver_ThreadClass::handleHeartbeat(mavlink_heartbeat_t& heartbeat)
{
    // We have a lot of messages types that we could handle, but only the arm state is important here
    if (heartbeat.type == MAV_TYPE_QUADROTOR) // Check if it has a chance to be our drone
    {
        // If drone is armed, this flag must be set
        bool isArmed = (heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;
        if (isArmed != (m_drone->motors == ARM))
        {
            LOG_F(INFO, "Drone motor state changed : Arm is now equals to %s", isArmed ? "true" : "false");
            m_drone->motors = isArmed ? ARM : UNARM;
            pdsChannels::state.chars[0] = isArmed ? 1 : 0;
        }
    }
}

void DroneReceiver_ThreadClass::handleAck(mavlink_command_ack_t& commandAck)
{
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    string buffer;
    buffer = " Command :" + to_string(m_drone->ack.command) + "\n";
    buffer = buffer + " result: " + to_string(m_drone->ack.result);
    buffer = buffer + " result_param2: " + to_string(m_drone->ack.result_param2) + "\n";
    buffer = buffer + " progress: " + to_string(m_drone->ack.progress) + "\n";
    cout << buffer << endl;
#endif

    m_drone->ack = commandAck;

    bool isResultAccepted = commandAck.result == MAV_RESULT_ACCEPTED;
    // Multiple sub commands to handle (change drone data + send messages to app)
    switch (commandAck.command)
    {
        // If arm / unarm, change the motor state
    case MAV_CMD_COMPONENT_ARM_DISARM:
        if (isResultAccepted)
        {
            LOG_F(INFO, "ARM_DISARM Command Accepted");
            m_drone->motors = m_drone->motors == ARM ? UNARM : ARM;
            // Normally, if the drone is unarmed, it must be landed too
            if (m_drone->motors == UNARM)
            {
                m_drone->tookOff = false;
            }
            pdsChannels::state.chars[0] = 1;
        }
        else if (commandAck.result != MAV_RESULT_IN_PROGRESS)
        {
            LOG_F(ERROR, "Arm / Disarm : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
        }
        break;
    case MAVLINK_MSG_ID_MANUAL_CONTROL:
        if (commandAck.result != MAV_RESULT_IN_PROGRESS && commandAck.result != MAV_RESULT_ACCEPTED)
        {
            LOG_F(ERROR, "Manual control : receive error of type %d : result2 = %d", commandAck.result, commandAck.result_param2);
        }
        break;
    }
}

void DroneReceiver_ThreadClass::handleAltitude(mavlink_altitude_t& altitude)
{
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    printf("Altitude : altitude_local %f altitude_relative %f altitude_terrain %f  bottom_clearance %f\n",
        altitude.altitude_local,
        altitude.altitude_relative,
        altitude.altitude_terrain,
        altitude.bottom_clearance
    );
#endif
    if (m_drone->altitude.time_usec > altitude.time_usec)
    {
        LOG_F(WARNING, "Received time usec before actual one (%ld vs %ld)",
            m_drone->altitude.time_usec,
            altitude.time_usec);
    }
    else
    {
        m_drone->altitude = altitude;
        pdsChannels::altitude.floats[0] = altitude.altitude_monotonic;
        pdsChannels::altitude.floats[1] = altitude.altitude_amsl;
        pdsChannels::altitude.floats[2] = altitude.altitude_local;
        pdsChannels::altitude.floats[3] = altitude.altitude_relative;
        pdsChannels::altitude.floats[4] = altitude.altitude_terrain;
        pdsChannels::altitude.floats[5] = altitude.bottom_clearance;
    }
}

void DroneReceiver_ThreadClass::handleAttitude(mavlink_attitude_t& attitude)
{
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    stringstream ss;
    ss << "attitude : [roll:" << attitude.roll
        << " pitch: " << attitude.pitch
        << " yaw: " << attitude.yaw
        << " rollspeed: " << attitude.rollspeed
        << " pitchspeed: " << attitude.pitchspeed
        << " yawspeed: " << attitude.yawspeed
        << "]\n";
    cout << ss.str();
#endif
    if (m_drone->attitude.time_boot_ms > attitude.time_boot_ms)
    {
        LOG_F(WARNING, "Received time boot ms before actual one (%d vs %d)",
            m_drone->attitude.time_boot_ms,
            attitude.time_boot_ms);
    }
    else
    {
        m_drone->attitude = attitude;
        pdsChannels::attitude.floats[0] = attitude.roll;
        pdsChannels::attitude.floats[1] = attitude.pitch;
        pdsChannels::attitude.floats[2] = attitude.yaw;
        pdsChannels::attitude.floats[3] = attitude.rollspeed;
        pdsChannels::attitude.floats[4] = attitude.pitchspeed;
        pdsChannels::attitude.floats[5] = attitude.yawspeed;
    }
}

void DroneReceiver_ThreadClass::handleLocalPosition(mavlink_local_position_ned_t& localPosition)
{
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    stringstream ss;
    ss << "global_position : [x:" << localPosition.x
        << " y: " << localPosition.y
        << " z: " << localPosition.z
        << " v(x,y,z): (" << localPosition.vx << "," << localPosition.vy << "," << localPosition.vz << ")"
        << "]\n";
    cout << ss.str();
#endif
    if (m_drone->local_position_ned.time_boot_ms > localPosition.time_boot_ms)
    {
        LOG_F(WARNING, "Received time boot ms before actual one (%d vs %d)",
            m_drone->local_position_ned.time_boot_ms,
            localPosition.time_boot_ms);
    }
    else
    {
        m_drone->local_position_ned = localPosition;
        pdsChannels::localPositionNed.floats[0] = localPosition.x;
        pdsChannels::localPositionNed.floats[1] = localPosition.y;
        pdsChannels::localPositionNed.floats[2] = localPosition.z;
        pdsChannels::localPositionNed.floats[3] = localPosition.vx;
        pdsChannels::localPositionNed.floats[4] = localPosition.vy;
        pdsChannels::localPositionNed.floats[5] = localPosition.vz;
    }
}

void DroneReceiver_ThreadClass::handleGlobalPosition(mavlink_global_position_int_t& globalPosition)
{
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    string buffer1 = "global_position : [lat:" + to_string(globalPosition.lat)
        + " lon: " + to_string(globalPosition.lon)
        + " alt: " + to_string(globalPosition.alt)
        + " alt_rel: " + to_string(globalPosition.relative_alt)
        + " v(x,y,z): (" + to_string(globalPosition.vx) + "," + to_string(globalPosition.vy) + "," + to_string(globalPosition.vz) + ")"
        + " hdg: " + to_string(globalPosition.hdg)
        + "]";
    cout << buffer1 << endl;
#endif
    if (m_drone->global_position_int.time_boot_ms > globalPosition.time_boot_ms)
    {
        LOG_F(WARNING, "Received time boot ms before actual one (%d vs %d)",
            m_drone->global_position_int.time_boot_ms,
            globalPosition.time_boot_ms);
    }
    else
    {
        m_drone->global_position_int = globalPosition;
        pdsChannels::globalPosition.ints32[0] = globalPosition.lat;
        pdsChannels::globalPosition.ints32[1] = globalPosition.lon;
        pdsChannels::globalPosition.ints32[2] = globalPosition.alt;
        pdsChannels::globalPosition.ints32[3] = globalPosition.relative_alt;
        pdsChannels::globalPosition.ints32[4] = globalPosition.vx;
        pdsChannels::globalPosition.ints32[5] = globalPosition.vy;
        pdsChannels::globalPosition.ints32[6] = globalPosition.vz;
        pdsChannels::globalPosition.ints32[7] = globalPosition.hdg;
    }
}

void DroneReceiver_ThreadClass::handleRawImu(mavlink_raw_imu_t& rawImu)
{
    // WARN : we don't store those structs, only for debugging
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    string buffer1 = "mavlink_raw_imu_t : Acc: " + to_string(rawImu.xacc) + " " + to_string(rawImu.yacc) + " " + to_string(rawImu.zacc) + "\n";
    buffer1 = buffer1 + " Gyro: " + to_string(rawImu.xgyro) + " " + to_string(rawImu.ygyro) + " " + to_string(rawImu.zgyro) + "\n";
    buffer1 = buffer1 + " Mag: " + to_string(rawImu.xmag) + " " + to_string(rawImu.ymag) + " " + to_string(rawImu.zmag) + "\n";
    cout << buffer1 << endl;
#endif
}

void DroneReceiver_ThreadClass::handleHighresImu(mavlink_highres_imu_t& highresImu)
{
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    string buffer1 = "highres : Acc: " + to_string(highresImu.xacc) + " " + to_string(highresImu.yacc) + " " + to_string(highresImu.zacc) + "\n";
    buffer1 = buffer1 + " Gyro: " + to_string(highresImu.xgyro) + " " + to_string(highresImu.ygyro) + " " + to_string(highresImu.zgyro) + "\n";
    buffer1 = buffer1 + " Mag: " + to_string(highresImu.xmag) + " " + to_string(highresImu.ymag) + " " + to_string(highresImu.zmag) + "\n";
    buffer1 = buffer1 + " Temperature: " + to_string(highresImu.temperature) + "\n\n";
    cout << buffer1 << endl;
#endif
    if (m_drone->highres_imu.time_usec > highresImu.time_usec)
    {
        LOG_F(WARNING, "Received time usec before actual one (%ld vs %ld)",
            m_drone->highres_imu.time_usec,
            highresImu.time_usec);
    }
    else
    {
        m_drone->highres_imu = highresImu;
        pdsChannels::highresImu.floats[0] = highresImu.xacc;
        pdsChannels::highresImu.floats[1] = highresImu.yacc;
        pdsChannels::highresImu.floats[2] = highresImu.zacc;
        pdsChannels::highresImu.floats[3] = highresImu.xgyro;
        pdsChannels::highresImu.floats[4] = highresImu.ygyro;
        pdsChannels::highresImu.floats[5] = highresImu.zgyro;
        pdsChannels::highresImu.floats[6] = highresImu.xmag;
        pdsChannels::highresImu.floats[7] = highresImu.ymag;
        pdsChannels::highresImu.floats[8] = highresImu.zmag;
        pdsChannels::highresImu.floats[9] = highresImu.abs_pressure;
        pdsChannels::highresImu.floats[10] = highresImu.diff_pressure;
        pdsChannels::highresImu.floats[11] = highresImu.pressure_alt;
        pdsChannels::highresImu.floats[12] = highresImu.temperature;
    }
}

void DroneReceiver_ThreadClass::handleBatteryStatus(mavlink_battery_status_t& batteryStatus)
{
#ifdef DRONERECEIVER_THREADCLASS_DEBUG
    cout << "Battery status : " << batteryStatus.battery_remaining << "\n";
#endif
    // No timestamp for this message
    m_drone->battery_status = batteryStatus;
    pdsChannels::battery.ints32[0] = batteryStatus.current_consumed;
    pdsChannels::battery.ints32[1] = batteryStatus.energy_consumed;
    pdsChannels::battery.ints32[2] = batteryStatus.temperature;
    pdsChannels::battery.ints32[3] = batteryStatus.current_battery;
    pdsChannels::battery.ints32[4] = batteryStatus.battery_remaining;
    pdsChannels::battery.ints32[5] = batteryStatus.time_remaining;
}