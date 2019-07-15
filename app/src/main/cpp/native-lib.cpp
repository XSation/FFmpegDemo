//
// Created by 薛凯 on 2019-07-03.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <random>
#include <helper/JavaCallHelper.h>
#include <player/MPlayer.h>
#include <android/native_window_jni.h>
#include "macro.h"


extern "C" {
#include <libavformat/avformat.h>
}

JavaVM *javaVM;
MPlayer *mPlayer;
ANativeWindow *window;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int JNI_OnLoad(JavaVM *vm, void *) {
    javaVM = vm;
    return JNI_VERSION_1_6;
}

//渲染图像到window
void renderCallBack(uint8_t *data, int linesize, int w, int h) {
    pthread_mutex_lock(&mutex);
    LOGE("renderCallBack1");
    if (!window) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    LOGE("renderCallBack2");

    ANativeWindow_setBuffersGeometry(window, w, h, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;
    if (ANativeWindow_lock(window, &windowBuffer, 0)) {
        ANativeWindow_release(window);
        window = 0;
        pthread_mutex_unlock(&mutex);
        return;
    }
    uint8_t *dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    int dst_linesize = windowBuffer.stride * 4;
    for (int i = 0; i < windowBuffer.height/2; ++i) {
        //全部复制
        memcpy(reinterpret_cast<void *>(dst_data + i * dst_linesize), data + i * linesize,
               static_cast<size_t>(dst_linesize));
    }
    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex);
}

extern "C"
JNIEXPORT void JNICALL Java_com_xk_ffmpegdemp_player_MPlayer_nprepare
        (JNIEnv *env, jobject jObject, jstring dataSource_, jstring cookie_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    const char *cookie = env->GetStringUTFChars(cookie_, 0);
    JavaCallHelper *javaCallHelper = new JavaCallHelper(javaVM, env, jObject);
    mPlayer = new MPlayer(dataSource, cookie, javaCallHelper);
    mPlayer->prepare();
    mPlayer->setRender(renderCallBack);
    env->ReleaseStringUTFChars(dataSource_, dataSource);
    env->ReleaseStringUTFChars(cookie_, cookie);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xk_ffmpegdemp_player_MPlayer_nstart(JNIEnv *env, jobject instance) {
    mPlayer->start();
}extern "C"
JNIEXPORT void JNICALL
Java_com_xk_ffmpegdemp_player_MPlayer_nsetSurface(JNIEnv *env, jobject instance, jobject surface) {
    pthread_mutex_lock(&mutex);
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&mutex);
}