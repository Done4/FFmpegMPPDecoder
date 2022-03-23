#include "H264MPPDecoder.h"
#include <unistd.h>

H264MPPDecoder::H264MPPDecoder(int width, int height)
{
    this->width = width;
    this->height = height;
}

H264MPPDecoder::~H264MPPDecoder()
{
    stopDecode();
}

bool H264MPPDecoder::init()
{
    MPP_RET ret = MPP_OK;
    MpiCmd mpi_cmd = MPP_CMD_BASE;
    MppParam param = NULL;
    MppCodingType type = MPP_VIDEO_CodingAVC;
    RK_U32 need_split = 1;
    
    //创建MPP context和MPP api 接口
    ret = mpp_create(&ctx, &mpi);
    if (MPP_OK != ret) 
    {
        std::cout<<"mpp_create faild"<<std::endl;
        stopDecode();
        return false;
    }
    //配置解码器 解码文件需要 split 模式
    mpi_cmd = MPP_DEC_SET_PARSER_SPLIT_MODE;
    param = &need_split;
    ret = mpi->control(ctx, mpi_cmd, param);
    if (MPP_OK != ret) 
    {
        std::cout<<"split control faild"<<std::endl;
        stopDecode();
        return false;
    }
    //初始化 MPP
    ret = mpp_init(ctx, MPP_CTX_DEC, type);
    if (MPP_OK != ret) 
    {
        std::cout<<"mpp_init faild"<<std::endl;
        stopDecode();
        return false;
    }

    return true;
}

void H264MPPDecoder::startDecode(AVPacket* packet)
{
    MppPacket mppPacket = NULL;
    MppFrame  frame  = NULL;
	// RK_U8 *base = NULL;
    
    mpp_packet_init(&mppPacket, packet->data, packet->size);
    mpp_packet_set_pts(mppPacket, packet->pts);
    if(MPP_OK != mpi->decode_put_packet(ctx, mppPacket))
    {
        return;
    }
    sleep(1);
    if (MPP_OK !=  mpi->decode_get_frame(ctx, &frame)) 
    {
        printf("decode_get_frame falied ret\n");
        return;
    }
    if(frame && yuvCallBack)
    {
        if (mpp_frame_get_info_change(frame)) 
        {
            printf("decode_get_frame get info changed found\n");
            mpi->control(ctx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
            return ;
        } 
        RK_U32 h_stride = mpp_frame_get_hor_stride(frame);
        RK_U32 v_stride = mpp_frame_get_ver_stride(frame);
        MppBuffer buffer = mpp_frame_get_buffer(frame);

        cv::Mat yuvImg(height * 3/2, width, CV_8UC1);
        RK_U8 *base = (RK_U8 *)mpp_buffer_get_ptr(buffer);
        RK_U8 *base_c = base + h_stride * v_stride;
        int idx = 0;
        for (int i = 0; i < height; i++, base += h_stride, idx += width) 
        {
            memcpy(yuvImg.data + idx, base, width);
        }
        for (int i = 0; i < height / 2; i++, base_c += h_stride, idx += width) 
        {
            memcpy(yuvImg.data + idx, base_c, width);
        }
        yuvCallBack(yuvImg);
        mpp_frame_deinit(&frame);
        frame = NULL;
    }

    mpp_packet_deinit(&mppPacket);
    mppPacket = NULL;
}

void H264MPPDecoder::stopDecode()
{
    if (ctx) 
    {
    	mpi->reset(ctx);
	mpp_destroy(ctx);
        ctx = NULL;
    }
}
