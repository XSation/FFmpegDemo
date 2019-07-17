//
// Created by 薛凯 on 2019-07-08.
//

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>

}

#include "VideoChannel.h"

/**
 * 丢已经解码的图片
 * @param q
 */
void dropAvFrame(queue<AVFrame *> &q) {
    if (!q.empty()) {
        AVFrame *frame = q.front();
        BaseChannel::releaseAVFrames(&frame);
        q.pop();
    }
}
VideoChannel::VideoChannel(int index, AVCodecContext *avCodecContext, AVRational rational, int fps)
        : BaseChannel(index,
                      avCodecContext, rational, fps) {
    this->fps = fps;
    avFrames.setSyncHandle(dropAvFrame);

}


void VideoChannel::play() {
    isPlaying = true;
    avPackets.setWork(true);
    avFrames.setWork(true);
    //1.解码 2.播放
    pthread_create(&tid_realplay, 0, task_realplay, this);
    pthread_create(&tid_decode, 0, task_decode, this);
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
    //定义一个指针数组，4个长度，可以存一个颜色信息RGBA
    __uint8_t *dst_data[4];
    int linesizes[4];
    //创建一个空的图像数据：dst_data. 他是一个指针数组。4个长度，分别是RGBA。
    av_image_alloc(dst_data, linesizes, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);
    double frame_delays = 1.0 / fps;

    while (isPlaying) {

        int ret = avFrames.pop(&avFrame);
        if (ret == 1) {

            sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(avFrame->data),
                      avFrame->linesize, 0,
                      avCodecContext->height, dst_data, linesizes);


            //获得 当前这一个画面 播放的相对的时间
            double clock = avFrame->best_effort_timestamp * av_q2d(time_base);
            //额外的间隔时间
            double extra_delay = avFrame->repeat_pict / (2 * fps);
            // 真实需要的间隔时间
            double delays = extra_delay + frame_delays;
            if (!audioChannel) {
                //休眠
//        //视频快了
//        av_usleep(frame_delays*1000000+x);
//        //视频慢了
//        av_usleep(frame_delays*1000000-x);
                av_usleep(delays * 1000000);
            } else {
                if (clock == 0) {
                    av_usleep(delays * 1000000);
                } else {
                    //比较音频与视频
                    double audioClock = audioChannel->clock;
                    //间隔 音视频相差的间隔
                    double diff = clock - audioClock;
                    if (diff > 0) {
                        //大于0 表示视频比较快
                        LOGE("视频快了：%lf", diff);
                        av_usleep((delays + diff) * 1000000);
                    } else if (diff < 0) {
                        //小于0 表示音频比较快
                        LOGE("音频快了：%lf", diff);
                        // 视频包积压的太多了 （丢包）
                        if (fabs(diff) >= 0.05) {
                            releaseAVFrames(&avFrame);
                            //丢包
                            avFrames.sync();
                            continue;
                        } else {
                            //不睡了 快点赶上 音频
                        }
                    }
                }
            }



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

void VideoChannel::setAudioChannel(AudioChannel *audiochannel) {
    this->audioChannel = audiochannel;

}

VideoChannel::~VideoChannel() = default;



