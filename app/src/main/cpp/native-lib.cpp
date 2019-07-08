//
// Created by 薛凯 on 2019-07-03.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <random>
#include <helper/JavaCallHelper.h>
#include "player/MPlayer.h"
#include "macro.h"


extern "C" {
#include <libavformat/avformat.h>
}

JavaVM *javaVM;

int JNI_OnLoad(JavaVM *vm, void *unused) {
    javaVM = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL Java_com_xk_ffmpegdemp_player_MPlayer_nprepare
        (JNIEnv *env, jobject jObject, jstring dataSource_, jstring cookie_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    const char *cookie = env->GetStringUTFChars(cookie_, 0);
    JavaCallHelper *javaCallHelper = new JavaCallHelper(javaVM, env, jObject);
    MPlayer *mPlayer = new MPlayer(dataSource, cookie, javaCallHelper);
    mPlayer->prepare();
//    mPlayer->_prepare();
//    av_version_info();
    env->ReleaseStringUTFChars(dataSource_, dataSource);
    env->ReleaseStringUTFChars(cookie_, cookie);
}
