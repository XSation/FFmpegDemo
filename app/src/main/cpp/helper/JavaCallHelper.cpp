//
// Created by 薛凯 on 2019-07-07.
//

#include <macro.h>
#include "JavaCallHelper.h"

JavaCallHelper::~JavaCallHelper() {
    this->jniEnv->DeleteGlobalRef(jObject);
}

JavaCallHelper::JavaCallHelper(JavaVM *javaVM, JNIEnv *jniEnv, jobject jObject) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jObject = jniEnv->NewGlobalRef(jObject);
    jclass clazz = jniEnv->GetObjectClass(jObject);
    onErrorMethodId = jniEnv->GetMethodID(clazz, "onError", "(ILjava/lang/String;)V");
    onPrepareMethodId = jniEnv->GetMethodID(clazz, "onPrepare", "()V");
}

void JavaCallHelper::onError(int errorCode, const char *msg, bool isJNIEnvThread) {
    if (isJNIEnvThread) {
        jniEnv->CallVoidMethod(jObject, onErrorMethodId, errorCode);
    } else {
        JNIEnv *jniEnv;
        javaVM->AttachCurrentThread(&jniEnv, 0);

        jniEnv->CallVoidMethod(jObject, onErrorMethodId, errorCode, jniEnv->NewStringUTF(msg));
        javaVM->DetachCurrentThread();
    }
}

void JavaCallHelper::onPrepare(bool isJNIEnvThread) {
    if (isJNIEnvThread) {
        jniEnv->CallVoidMethod(jObject, onPrepareMethodId);
    } else {
        JNIEnv *jniEnv;
        javaVM->AttachCurrentThread(&jniEnv, 0);
        jniEnv->CallVoidMethod(jObject, onPrepareMethodId);
        javaVM->DetachCurrentThread();
    }
}
