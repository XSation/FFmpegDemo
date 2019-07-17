//
// Created by 薛凯 on 2019-07-09.
//

#ifndef FFMPEGDEMO_BASHCHANNEL_H
#define FFMPEGDEMO_BASHCHANNEL_H


#include "utils/safe_queue.h"
#include <macro.h>

extern "C" {
#include <libavcodec/avcodec.h>
};

class BaseChannel {

public:
    BaseChannel(int index, AVCodecContext *avCodecContext, AVRational rational, int fps) : index(
            index),
                                                                                           avCodecContext(
                                                                                                   avCodecContext),
                                                                                           time_base(
                                                                                                   rational),
                                                                                           fps(fps) {}

    int index;
    double clock;
    safe_queue<AVPacket *> avPackets;

    safe_queue<AVFrame *> avFrames;

    virtual ~BaseChannel() {
        avPackets.setReleaseCallback(releaseAVPackets);
        avPackets.clear();
        avFrames.setReleaseCallback(releaseAVFrames);
        avFrames.clear();
    }

    static void releaseAVPackets(AVPacket **avPacket) {
        if (avPacket) {
            av_packet_free(avPacket);
            *avPacket = 0;
        }
    }

    static void releaseAVFrames(AVFrame **avFrame) {
        if (avFrame) {
            av_frame_free(avFrame);
            *avFrame = 0;
        }
    }

    virtual void play() = 0;

    void _decode() {
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

    AVCodecContext *avCodecContext;
    AVRational time_base;
    int fps;
protected:
    //解码线程
    pthread_t tid_decode;
    //播放线程
    pthread_t tid_realplay;
    bool isPlaying;

    virtual void _realPlay() = 0;

    static void *task_decode(void *args) {
        auto *videoChannel = static_cast<BaseChannel *>(args);
        videoChannel->_decode();
        return 0;
    }

    static void *task_realplay(void *args) {
        auto *videoChannel = static_cast<BaseChannel *>(args);
        videoChannel->_realPlay();
        return 0;
    }
};

#endif //FFMPEGDEMO_BASHCHANNEL_H
