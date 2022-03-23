#ifndef _DECODER_H_
#define _DECODER_H_

#include <iostream>
#include <functional>
#include <vector>
#include <opencv2/core.hpp>
extern "C"{
    #include <libavcodec/avcodec.h>
}

class Decoder
{
public:
    Decoder(){}
    virtual ~Decoder(){}
    virtual void startDecode(AVPacket*) = 0;
    virtual void stopDecode() = 0;
    virtual void setDecodeCallBack(std::function<void(cv::Mat &yuvImg)> oncall) = 0;
    virtual bool init() = 0;
};

#endif