//
// Created by 薛凯 on 2019-07-03.
//

//
// Created by 薛凯 on 2019-07-03.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <random>

extern "C" {
#include <libavformat/avformat.h>
}

#include <MPlayer.h>
#include <macro.h>
#include <iostream>
#include <helper/JavaCallHelper.h>
#include "MPlayer.h"


using namespace std;

void *task_prepare(void *args) {
    auto *mPlayer = static_cast<MPlayer *>(args);
    mPlayer->_prepare(false);
    return 0;
}

MPlayer::MPlayer(const char *dataSource, const char *cookie, JavaCallHelper *pHelper) {
    this->javaCallHelper = pHelper;
    mDataSource = new char[strlen(dataSource)];
    strcpy(mDataSource, dataSource);
    mCookie = new char[strlen(cookie)];
    strcpy(mCookie, cookie);
}

MPlayer::~MPlayer() {
    DELETE(mDataSource);
    DELETE(mCookie);
    DELETE(javaCallHelper);
    DELETE(headers);
}

void MPlayer::prepare() {

    //通过数据源，来准备一些信息，比如视频大小之类的。利用ffmpeg，可能会走网络，所以要开线程
    pthread_create(&tid, 0, task_prepare, this);
}

void MPlayer::_prepare(bool isJNIEnvThread) {
    avformat_network_init();
    //里面会包含视频的宽高等信息。
    AVFormatContext *pAVFormatContext = nullptr;
    AVDictionary *codec_opts = NULL;
    headers = new char[strlen("Cookie:") + strlen(mCookie)];
    strcpy(headers, "Cookie:");
    strcat(headers, mCookie);
    av_dict_set(&codec_opts, "headers", headers, 0);
    int retCode = avformat_open_input(&pAVFormatContext, mDataSource, 0, &codec_opts);
    if (retCode) {//不等于0表示成功
        LOGE("获取资源失败，错误码：%d %s", retCode, av_err2str(retCode));
        javaCallHelper->onError(FFMPEG_CAN_NOT_OPEN_URL, "打开资源失败", isJNIEnvThread);
        return;
    }
    //查找音视频流相关的信息，给context的stream等成员赋值
    retCode = avformat_find_stream_info(pAVFormatContext, 0);
    if (retCode < 0) {//等于0表示成功
        LOGE("查找流失败，错误码：%d %s", retCode, av_err2str(retCode));
        //反射回调到java中
        javaCallHelper->onError(FFMPEG_CAN_NOT_FIND_STREAMS, "查找流失败", isJNIEnvThread);
        return;
    }
    LOGE("获取资源成功，流的数目为：%d", pAVFormatContext->nb_streams);
    for (int i = 0; i < pAVFormatContext->nb_streams; ++i) {
        //代表一个音频流、视频流、甚至字幕流
        AVStream *avStream = pAVFormatContext->streams[i];
        //解码这段流的各种参数信息（宽高、码率、帧率等）
        AVCodecParameters *avCodecParameters = avStream->codecpar;
        LOGE("流类型：%s", av_get_media_type_string(avCodecParameters->codec_type));
        //无论音视频，都需要以下几步
        //1.通过codec_id找到一个解码器（可能编译出来的ffmpeg不支持这种解码器）
        AVCodec *codec = avcodec_find_decoder(avCodecParameters->codec_id);
        if (!codec) {
            javaCallHelper->onError(FFMPEG_FIND_DECODER_FAIL, "查找解码器失败", isJNIEnvThread);
            return;
        }
        //2.创建解码器上下文
        AVCodecContext *avCodecContext = avcodec_alloc_context3(codec);
        if (!avCodecContext) {
            javaCallHelper->onError(FFMPEG_ALLOC_CODEC_CONTEXT_FAIL, "解码器上下文创建失败", isJNIEnvThread);
            return;
        }
        //3.解码的各种参数，传递给解码器的上下文
        retCode = avcodec_parameters_to_context(avCodecContext, avCodecParameters);
        if (retCode < 0) {
            javaCallHelper->onError(FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL, "设置解码器上下文参数失败",
                                    isJNIEnvThread);
            return;
        }
        //4.打开编码器
        retCode = avcodec_open2(avCodecContext, codec, 0);
        if (retCode != 0) {
            javaCallHelper->onError(FFMPEG_OPEN_DECODER_FAIL, "打开解码器失败",
                                    isJNIEnvThread);
            return;
        }
        //创建Video/Audio Channel。
        if (avCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoChannel = new VideoChannel();
        } else if (avCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioChannel = new AudioChannel();
        }
    }
    //有一个不存在，就抛出error
    if (!audioChannel && !videoChannel) {
        javaCallHelper->onError(FFMPEG_NOMEDIA, "没有音视频",
                                isJNIEnvThread);
        return;
    }
    //准备完毕，随时可以播放了
    javaCallHelper->onPrepare(isJNIEnvThread);
}

