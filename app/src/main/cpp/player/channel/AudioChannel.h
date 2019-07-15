//
// Created by 薛凯 on 2019-07-08.
//

#ifndef FFMPEGDEMO_AUDIOCHANNEL_H
#define FFMPEGDEMO_AUDIOCHANNEL_H


#include "BaseChannel.h"

class AudioChannel : public BaseChannel {
public:
    AudioChannel(int index, AVCodecContext *avCodecContext);

    void play();

};


#endif //FFMPEGDEMO_AUDIOCHANNEL_H
