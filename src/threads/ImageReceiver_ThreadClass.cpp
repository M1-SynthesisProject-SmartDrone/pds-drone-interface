#include "ImageReceiver_ThreadClass.h"

#include <chrono>

#include <loguru/loguru.hpp>

#include "channels/channels.h"

using namespace std;

ImageReceiver_ThreadClass::ImageReceiver_ThreadClass(int deviceId, int framerate)
    : Abstract_ThreadClass("Image receiver",  1000.0 / (double) framerate, 1000.0 / (double) framerate),
    m_deviceId(deviceId), m_framerate(framerate)
{
    
}

ImageReceiver_ThreadClass::~ImageReceiver_ThreadClass()
{}

void ImageReceiver_ThreadClass::run()
{
    initRun();

    // chrono::steady_clock::time_point startPoint, endPoint;
    cv::Mat image;
    cv::VideoCapture capture(m_deviceId);
    if (!capture.isOpened())
    {
        LOG_F(ERROR, "Cannot open camera for capture");
        throw runtime_error("Cannot open camera for capture");
    }

    while (isRunFlag())
    {
        onStartLoop();
        capture >> image;
        
        auto data = image.data;
        int size = image.total() * image.elemSize();
        int dataSize = size * sizeof(uchar);

        memcpy(pdsChannels::image.uchars, data, dataSize);
        pdsChannels::imageSize.uints16[0] = dataSize;

        onEndLoop();
    }
}
