//
// Created by 薛凯 on 2019-07-03.
//

#ifndef FFMPEGDEMO_MPLAYER_H
#define FFMPEGDEMO_MPLAYER_H


#include <helper/JavaCallHelper.h>
#include <channel/VideoChannel.h>
#include <channel/AudioChannel.h>

extern "C" {
#include <libavformat/avformat.h>
}

class MPlayer {
public:
    MPlayer(const char *dataSource, const char *cookie, JavaCallHelper *pHelper);

    ~MPlayer();

    void prepare();

    void _prepare(bool isJNIEnvThread);

    void start();

    void _start();

    void setRender(Render renderCallback);

private:

    Render renderCallback;
    //数据源
    char *mDataSource;
    char *mCookie;
    pthread_t tid_prepare;
    pthread_t tid_start;
    char *headers;
    JavaCallHelper *javaCallHelper;
    //如果这里不写=0，在进行if(!videoChannel)的判断的时候，可能就有问题了
    VideoChannel *videoChannel = 0;
    AudioChannel *audioChannel = 0;
    //里面会包含视频的宽高等信息。
    AVFormatContext *pAVFormatContext = 0;
};


#endif //FFMPEGDEMO_MPLAYER_H
