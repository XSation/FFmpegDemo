//
// Created by 薛凯 on 2019-07-08.
//

#ifndef FFMPEGDEMO_AUDIOCHANNEL_H
#define FFMPEGDEMO_AUDIOCHANNEL_H

extern "C" {
#include <libswresample/swresample.h>
};

#include "BaseChannel.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
//typedef void(*Render)(uint8_t *data, int linesize, int w, int h);

class AudioChannel : public BaseChannel {
public:
    AudioChannel(int index, AVCodecContext *avCodecContext);

    virtual ~AudioChannel();

    void play();

    void _realPlay();

    int getPcm();

    uint8_t *data = 0;
    int out_channels;
    int out_samplesize;
    int out_sample_rate;
private:
    /**
     * OpenSL ES
     */
    // 引擎与引擎接口
    SLObjectItf engineObject = 0;
    SLEngineItf engineInterface = 0;
    //混音器
    SLObjectItf outputMixObject = 0;
    //播放器
    SLObjectItf bqPlayerObject = 0;
    //播放器接口
    SLPlayItf bqPlayerInterface = 0;

    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueueInterface = 0;
    //重采样
    SwrContext *swrContext = 0;

};


#endif //FFMPEGDEMO_AUDIOCHANNEL_H
