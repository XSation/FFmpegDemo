//
// Created by 薛凯 on 2019-07-09.
//

#ifndef FFMPEGDEMO_BASHCHANNEL_H
#define FFMPEGDEMO_BASHCHANNEL_H


#include "utils/safe_queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

class BaseChannel {
public:
    BaseChannel(int index, AVCodecContext *avCodecContext) : index(index),
                                                             avCodecContext(avCodecContext) {}

    int index;
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

    AVCodecContext *avCodecContext;
};

#endif //FFMPEGDEMO_BASHCHANNEL_H
