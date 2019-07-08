//
// Created by 薛凯 on 2019-07-03.
//

#ifndef FFMPEGDEMO_MPLAYER_H
#define FFMPEGDEMO_MPLAYER_H


#include <helper/JavaCallHelper.h>
#include <channel/VideoChannel.h>
#include <channel/AudioChannel.h>

class MPlayer {
public:
    MPlayer(const char *dataSource, const char *cookie, JavaCallHelper *pHelper);

    ~MPlayer();

    void prepare();

    void _prepare(bool isJNIEnvThread);


private:

    //数据源
    char *mDataSource;
    char *mCookie;
    pthread_t tid;
    char *headers;
    JavaCallHelper *javaCallHelper;
    VideoChannel *videoChannel;
    AudioChannel *audioChannel;
};


#endif //FFMPEGDEMO_MPLAYER_H
