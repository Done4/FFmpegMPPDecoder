#include "H264FFmpegDecoder.h"

H264FFmpegDecoder::H264FFmpegDecoder(AVFormatContext *pFormatCtx, int videoIndex, int width, int height)
{
    this->pFormatCtx = pFormatCtx;
    this->videoIndex = videoIndex;
    this->width = width;
    this->height = height;
}

H264FFmpegDecoder::~H264FFmpegDecoder()
{
	stopDecode();
}

bool H264FFmpegDecoder::init()
{
    // 申请帧图像的内存空间
	pFrame = av_frame_alloc();
 
	// 解码器指针
	AVCodec* pCodec{nullptr};

	// 创建码流对应的解码器上下文
	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (pCodecCtx == NULL) 
	{
		std::cout << "Could not allocate AVCodecContext" << std::endl;
		stopDecode();
		return false;
	}
	avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoIndex]->codecpar);
	// 找到码流对应的decoder
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if(!pCodec) 
	{
		std::cout << "Unsupported codec!" << std::endl;
		stopDecode();
		return false;
	}

	// 打开解码器
	if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) 
	{
		std::cout << "open codec faild!" << std::endl;
		stopDecode();
		return false;
	}

    return true;
}

void H264FFmpegDecoder::startDecode(AVPacket* packet)
{
    avcodec_send_packet(pCodecCtx, packet);
    if(avcodec_receive_frame(pCodecCtx, pFrame) == 0)
    {
        if(yuvCallBack)
        {
            cv::Mat yuvImg(height * 3/2, width, CV_8UC1);
            int y = height * width;
            memcpy(yuvImg.data, pFrame->data[0], y);
            memcpy(yuvImg.data + y, pFrame->data[1], y/4);
            memcpy(yuvImg.data + 5*y/4, pFrame->data[2], y/4);
            //返回YUV420P
            yuvCallBack(yuvImg);
        }
    }
}

void H264FFmpegDecoder::stopDecode()
{
    if(pCodecCtx) 
	{
		avcodec_close(pCodecCtx);
		pCodecCtx = nullptr;
	}
    if(pFrame)
	{
		av_frame_free(&pFrame);
		pFrame = nullptr;
	}
}