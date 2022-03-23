#include "Decoder.h"
#include "H264FFmpegDecoder.h"
#include "H264MPPDecoder.h"
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

 //yuv420p转rgB
void YUV420P2RGB(cv::Mat yuvImg, cv::Mat &rgbImg)
{
        cv::cvtColor(yuvImg, rgbImg, CV_YUV420p2RGB);
}

//yuv420sp转rgB
void YUV420SP2RGB(cv::Mat yuvImg, cv::Mat &rgbImg)
{
        cv::cvtColor(yuvImg, rgbImg, CV_YUV420sp2RGB);
}

int main()
{
     int width = 640;
     int height = 360;
    AVFormatContext *pFormatCtx = NULL;
    std::string inAvFile = "rtsp://xxxxx";
    avformat_network_init();
    AVDictionary* options = NULL;
	av_dict_set(&options, "stimeout", "10000000", 0);
	if (avformat_open_input(&pFormatCtx, inAvFile.c_str(), 0, &options) < 0) {
		std::cout << "Could not open input file:" << inAvFile << std::endl;
		av_dict_free(&options);
        return 0;
	}
	av_dict_free(&options);

    // 读取输入流的基本信息到pFormatCtx中
	if (avformat_find_stream_info(pFormatCtx, 0) < 0) {
		if(pFormatCtx) {
			avformat_close_input(&pFormatCtx);
			pFormatCtx = nullptr;
		}
		std::cout << "Failed to retrieve input stream information" << std::endl;
        return 0;
	}

    int videoIndex = -1;
    if(pFormatCtx)
    {
        for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++) 
        {
                if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                    videoIndex = i;
                    break;
                }
        }
    }
        
    if(videoIndex == -1)
    {
        std::cout<<"Didn't find a video stream"<<std::endl;
        return 0;
    }
    
    AVPacket *av_packet = NULL;
    av_packet = (AVPacket *)av_malloc(sizeof(AVPacket)); // 申请空间
    //软解
    // Decoder *decoder = new H264FFmpegDecoder(pFormatCtx, videoIndex, width, height);
    // decoder->setDecodeCallBack([&](cv::Mat &yuvImg){
    //     cv::Mat rgbImg(height, width, CV_8UC3);
    //     YUV420P2RGB(yuvImg, rgbImg);
    //     cv::imwrite("yuv420.jpg", rgbImg);
    // });
    
    // 硬解
    Decoder *decoder = new H264MPPDecoder(width, height);
    decoder->setDecodeCallBack([&](cv::Mat &yuvImg){
        cv::Mat rgbImg(height, width, CV_8UC3);
        YUV420SP2RGB(yuvImg, rgbImg);
        cv::imwrite("yuv420sp.jpg", rgbImg);
    });
    if(decoder->init())
    {
        while(1)
        {
            if(av_read_frame(pFormatCtx, av_packet) == 0)
            {
                if(av_packet->stream_index == videoIndex)
                {
                    decoder->startDecode(av_packet);
                }
            }
        }
    }
   

    delete decoder;
    decoder = NULL;

    av_free(av_packet);
    avformat_close_input(&pFormatCtx);

    return 0;
}
