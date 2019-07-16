//
// Created by 薛凯 on 2019-07-08.
//

#ifndef FFMPEGDEMO_VIDEOCHANNEL_H
#define FFMPEGDEMO_VIDEOCHANNEL_H


#include "BaseChannel.h"
#include <macro.h>

typedef void(*Render)(uint8_t *data, int linesize, int w, int h);

class VideoChannel : public BaseChannel {
public:
    VideoChannel(int index, AVCodecContext *avCodecContext);

    virtual ~VideoChannel();

    void play();

    void _realPlay();

    void setRender(Render renderCallback);

private:

    Render renderCallback;

};


#endif //FFMPEGDEMO_VIDEOCHANNEL_H
