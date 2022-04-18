#include "channels.h"

namespace pdsChannels
{
    blc_channel controlCommands("/pdsDrone.control.commands", BLC_CHANNEL_READ, 'INT8', 'NDEF', 1, 2);
    // blc_channel controlMotors("/pdsDrone.control.motors", BLC_CHANNEL_READ, 'FL32', 'NDEF', { 1, 4 });


    // blc_channel highresImu("/pdsDrone.sensors.highres", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', { 1, 13 });
    // blc_channel heartbeat("/pdsDrone.heartbeat", BLC_CHANNEL_WRITE, 'INT8', 'NDEF', { 1, 5 });
    // blc_channel attitude("/pdsDrone.sensors.attitude", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', { 1, 6 });
    // blc_channel localPositionNed("/pdsDrone.sensors.local_position", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', { 1, 6 });
    // blc_channel globalPosition("/pdsDrone.sensors.global_position", BLC_CHANNEL_WRITE, 'INT32', 'NDEF', { 1, 8 });
}