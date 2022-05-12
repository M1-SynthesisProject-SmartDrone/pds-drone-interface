#include "channels.h"

namespace pdsChannels
{
    blc_channel controlCommands("/pdsDrone.control.commands", BLC_CHANNEL_READ, 'INT8', 'NDEF', 1, 3);
    blc_channel controlMotors("/pdsDrone.control.motors", BLC_CHANNEL_READ, 'FL32', 'NDEF', 1, 4);

    blc_channel state("/pdsDrone.state", BLC_CHANNEL_WRITE, 'INT8', 'NDEF', 1, 1);
    blc_channel highresImu("/pdsDrone.sensors.highres", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', 1, 13);
    blc_channel heartbeat("/pdsDrone.heartbeat", BLC_CHANNEL_WRITE, 'INT8', 'NDEF', 1, 5);
    blc_channel altitude("/pdsDrone.sensors.altitude", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', 1, 6);
    blc_channel attitude("/pdsDrone.sensors.attitude", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', 1, 6);
    blc_channel localPositionNed("/pdsDrone.sensors.local_position", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', 1, 6);
    // 'INT32' is not a valid value (compiler error), and FL32 does the same
    blc_channel globalPosition("/pdsDrone.sensors.global_position", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', 1, 8);
    blc_channel battery("/pdsDrone.sensors.battery", BLC_CHANNEL_WRITE, 'FL32', 'NDEF', 1, 6);
    blc_channel image("/pdsDrone.camera.image", BLC_CHANNEL_WRITE, 'UIN8', 'NDEF', 1, 4096);

    void closeChannels()
    {
        state.remove();
        controlCommands.remove();
        controlMotors.remove();
        highresImu.remove();
        heartbeat.remove();
        altitude.remove();
        attitude.remove();
        localPositionNed.remove();
        globalPosition.remove();
        battery.remove();
        image.remove();
    }
}