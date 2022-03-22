### 项目使用FFmpeg拉去rtsp流进行解码
可选用FFmpeg进行软解和MPP进行硬解
使用方法
必须有的init，然后就是设置回调函数，自定义自己的操作
通过解码回调出来的都是YUV图像，软解出来的是YUV420P，硬解出来的是YUV420SP
```C++
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
```
