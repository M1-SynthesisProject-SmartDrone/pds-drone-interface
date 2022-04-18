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
     * 2 int8 (char) values : ARM & DISARM
     */
    extern blc_channel controlCommands;
    /**
     * 4 float fields : x, y, z, r (between -1 and 1)
     */
    extern blc_channel controlMotors;


    // ==== INFOS CHANNELS ====
    /**
     * 13 float fields, see mavlink_highres_imu_t structure
     */
    extern blc_channel highresImu;

    /**
     * 5 char fields : see mavlink_heartbeat_t
     */
    extern blc_channel heartbeat;

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