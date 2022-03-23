#ifndef _H264FFMPEGDECODER_H_
#define _H264FFMPEGDECODER_H_

#include "Decoder.h"
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
}

class H264FFmpegDecoder : public Decoder
{
private:
    AVFormatContext* pFormatCtx{nullptr};	            ///< ffmpeg输入上下文
    AVCodecContext* pCodecCtx{nullptr};		            ///< 解码器指针
    AVFrame* pFrame{nullptr};			                ///< 视频帧图像
    int width;		                    	            ///< 视频宽度
    int height;		                                    ///< 视频高度
    int videoIndex;
    std::function<void(cv::Mat &yuvImg)> yuvCallBack;  ///<解码回調

public:
    H264FFmpegDecoder(AVFormatContext* pFormatCtx, int videoIndex, int width, int height);
    ~H264FFmpegDecoder();
    bool init();
    void startDecode(AVPacket*);
    void stopDecode();
    void setDecodeCallBack(std::function<void(cv::Mat &yuvImg)> oncall){yuvCallBack = oncall;}
};

#endif