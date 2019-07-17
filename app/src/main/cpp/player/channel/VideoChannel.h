//
// Created by 薛凯 on 2019-07-08.
//

#ifndef FFMPEGDEMO_VIDEOCHANNEL_H
#define FFMPEGDEMO_VIDEOCHANNEL_H


#include "BaseChannel.h"
#include "AudioChannel.h"
#include <macro.h>

typedef void(*Render)(uint8_t *data, int linesize, int w, int h);

class VideoChannel : public BaseChannel {
public:
    VideoChannel(int index, AVCodecContext *avCodecContext, AVRational rational, int i);

    virtual ~VideoChannel();

    void play();

    void _realPlay();

    void setRender(Render renderCallback);

    void setAudioChannel(AudioChannel *audiochannel);

private:
    AudioChannel *audioChannel;
    Render renderCallback;


};


#endif //FFMPEGDEMO_VIDEOCHANNEL_H
