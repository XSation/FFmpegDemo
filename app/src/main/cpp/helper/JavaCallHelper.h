//
// 提供回调java的方法
// Created by 薛凯 on 2019-07-07.
//

#ifndef FFMPEGDEMO_JAVACALLHELPER_H
#define FFMPEGDEMO_JAVACALLHELPER_H


#include <jni.h>

class JavaCallHelper {
public:
    JavaCallHelper(JavaVM *javaVM, JNIEnv *jniEnv, jobject jObject);

    virtual ~JavaCallHelper();

    /**
     * native中调用这个方法，这个方法再去反射java方法
     * @param isJNIEnvThread 是否和JNIEnv在同一个线程中
     */
    void onError(int errorCode, const char *msg, bool isJNIEnvThread);

    /**
     * native中调用这个方法，这个方法再去反射java方法
     * @param isJNIEnvThread 是否和JNIEnv在同一个线程中
     */
    void onPrepare(bool isJNIEnvThread);

private:
    JavaVM *javaVM;
    //JNIEnv不能跨线程调用，所以该JNIEnv和调用的不在一个线程时，不要用了，让javaVM重新attach一个新的jniEnv出来
    JNIEnv *jniEnv;
    jobject jObject;
    jmethodID onErrorMethodId;
    jmethodID onPrepareMethodId;

};


#endif //FFMPEGDEMO_JAVACALLHELPER_H
