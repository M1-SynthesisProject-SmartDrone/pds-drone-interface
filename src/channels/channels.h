#ifndef __CHANNELS_H__
#define __CHANNELS_H__

#include <blc_channel.h>

/**
 * For each channel, we describe :
 *  - the type of value contained in
 *  - how many fields it contains (the array length in other words)
 *  - what each field mean
 */

namespace pdsChannels
{
    // ==== COMMAND CHANNELS ====
    /**
     * 3 int8 (char) values : ARM, DISARM and TAKE_OFF
     */
    extern blc_channel controlCommands;
    /**
     * 4 float fields : x, y, z, r (between -1 and 1)
     */
    extern blc_channel controlMotors;


    // ==== INFOS CHANNELS ====
    /**
     * 1 int8 (char) field :
     *  - isArmed
     */
    extern blc_channel state;

    /**
     * 13 float fields, see mavlink_highres_imu_t structure
     */
    extern blc_channel highresImu;

    /**
     * 5 char fields : see mavlink_heartbeat_t
     */
    extern blc_channel heartbeat;

    /**
     * 6 float fields : see mavlink_altitude_t
     */
    extern blc_channel altitude;

    /**
     * 6 float fields : see mavlink_attitude_t
     */
    extern blc_channel attitude;

    /**
     * 6 float fields : see mavlink_local_position_ned_t
     */
    extern blc_channel localPositionNed;

    /**
     * 8 int32 fields : see mavlink_global_position_int_t
     */
    extern blc_channel globalPosition;

    /**
     * 6 int32 fields :
     *  - int32_t current_consumed  [mAh] Consumed charge, -1: autopilot does not provide consumption estimate
     *  - int32_t energy_consumed [hJ] Consumed energy, -1: autopilot does not provide energy consumption estimate
     *  - int16_t temperature [cdegC] Temperature of the battery. INT16_MAX for unknown temperature.
     *  - int16_t current_battery [cA] Battery current, -1: autopilot does not measure the current
     *  - int8_t battery_remaining [%] Remaining battery energy. Values: [0-100], -1: autopilot does not estimate the remaining battery.
     *  - int32_t time_remaining [s] Remaining battery time, 0: autopilot does not provide remaining battery time estimate
     */
    extern blc_channel battery;

    /**
     * The image captured by the camera, stored in a unsigned char 
     * array of size 4mb
     */
    extern blc_channel image;
    /**
     * 1 uint32 field containing the number of bytes written in the channel "image"
     */
    extern blc_channel imageSize;

    // ==== FUNCTIONS ====
    void closeChannels();
}

/**
 * Here are all channels types that we can define (extract of blc_core library source code)
 * See : https://git.cyu.fr/blaar/blibs/blc_core/-/blob/master/src/blc_tools.cpp
 * uint32_t type_str;
 *   switch (type) {
 *       case 'UIN8':case 'INT8':return 1;
 *           break;
 *       case 'UI16':case 'IN16': return 2;
 *           break;
 *       case 'UI32':case 'IN32':case 'FL32': return 4;
 *           break;
 *       case 'UI64':case 'IN64':case 'FL64': return 8;
 *           break;
 *       case 'CHAR': case 'TEXT': EXIT_ON_ERROR("The type '%.4s' is unknown. You probably mean 'INT8'", UINT32_TO_STRING(type_str, type));
 *       default:EXIT_ON_ERROR("Unknonw type '%.4s'.", UINT32_TO_STRING(type_str, type));
 *           break;
 *   }
 *   return 0;
 */

#endif // __CHANNELS_H__