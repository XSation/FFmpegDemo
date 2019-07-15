//
// Created by 薛凯 on 2019-07-08.
//

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
}

#include "VideoChannel.h"


void *task_decode(void *args) {
    auto *videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->_decode();
    return 0;
}

void *task_realplay(void *args) {
    auto *videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->_realPlay();
    return 0;
}

VideoChannel::VideoChannel(int index, AVCodecContext *avCodecContext) : BaseChannel(index,
                                                                                    avCodecContext) {
}


void VideoChannel::play() {
    isPlaying = true;
    //1.解码 2.播放
    pthread_create(&tid_realplay, 0, task_realplay, this);
    pthread_create(&tid_decode, 0, task_decode, this);
}

void VideoChannel::_decode() {
    //循环从avPackets中读，并且解码成AVFrame塞到队列中
    AVPacket *avPacket;
    while (isPlaying) {
        LOGE("decode");
        int ret = avPackets.pop(&avPacket);
        if (ret == 1) {
            //pop成功
            //把AVPacket发送给解码器。后续调用receive，从解码器中拿AVFrame
            ret = avcodec_send_packet(avCodecContext, avPacket);
            av_packet_free(&avPacket);
            if (ret == AVERROR(EAGAIN)) {
                //大概是由于解码器里的avpacket太多了，导致解码失败，所以继续receive
            } else if (ret != 0) {
                //失败
                break;
            }

            if (!isPlaying) {
                break;
            }

            AVFrame *avFrame = av_frame_alloc();
            //从解码器中读取AVFrame
            ret = avcodec_receive_frame(avCodecContext, avFrame);
            if (ret == AVERROR(EAGAIN)) {
                //大概是由于解码器里的avpacket太多了，导致解码失败，所以继续receive
            } else if (ret != 0) {
                //失败
                break;
            }
            avFrames.push(avFrame);
        }
    }
    av_packet_free(&avPacket);
}


void VideoChannel::_realPlay() {
    //从AVFrames中读取，并且转成可以显示的数据
    AVFrame *avFrame;
    //获取一个context，用来将视频源的图像格式转为BGRA
    SwsContext *swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                            avCodecContext->pix_fmt,
                                            avCodecContext->width, avCodecContext->height,
                                            AV_PIX_FMT_RGBA,
                                            SWS_BILINEAR,//缩放的算法，重视速度还是质量等等
                                            0, 0, 0);
    LOGE("_realPlay1");
    //定义一个指针数组，4个长度，可以存一个颜色信息RGBA
    __uint8_t *dst_data[4];
    int linesizes[4];
    //创建一个空的图像数据：dst_data. 他是一个指针数组。4个长度，分别是RGBA。
    av_image_alloc(dst_data, linesizes, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);
    LOGE("_realPlay2");

    while (isPlaying) {
        LOGE("_realPlay3");

        int ret = avFrames.pop(&avFrame);
        if (ret == 1) {
            LOGE("_realPlay4");

            sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(avFrame->data),
                      avFrame->linesize, 0,
                      avCodecContext->height, dst_data, linesizes);
            LOGE("_realPlay5");
            //回调出去
            renderCallback(dst_data[0],
                           linesizes[0],
                           avCodecContext->width,
                           avCodecContext->height);
            LOGE("_realPlay6");
            releaseAVFrames(&avFrame);
        }
    }
    av_freep(dst_data[0]);
    releaseAVFrames(&avFrame);
}

void VideoChannel::setRender(Render renderCallback) {
    this->renderCallback = renderCallback;
}

VideoChannel::~VideoChannel() = default;



