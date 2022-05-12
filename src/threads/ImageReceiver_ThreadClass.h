#ifndef __IMAGERECEIVER_THREADCLASS_H__
#define __IMAGERECEIVER_THREADCLASS_H__

#include <opencv2/opencv.hpp>

#include "Abstract_ThreadClass.h"

/**
 * Thsi thread will contiiously capture the image from the camera 
 * wanted and store it in the appropriate channel
 */
class ImageReceiver_ThreadClass : public Abstract_ThreadClass
{
private:
    int m_deviceId;
    int m_framerate;

    // In case of we want opmtimization of space (but we will lose time)
    // const std::vector<int> COMPRESSION_PARAMS{ 
    //     cv::IMWRITE_PNG_COMPRESSION, 1, 
    //     cv::IMWRITE_PNG_STRATEGY, cv::IMWRITE_PNG_STRATEGY_DEFAULT
    // };

public:
    ImageReceiver_ThreadClass(int deviceId, int framerate);
    ~ImageReceiver_ThreadClass();

    void run();
};

#endif // __IMAGERECEIVER_THREADCLASS_H__