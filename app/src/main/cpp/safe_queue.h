//
// Created by 薛凯 on 2019-06-28.
//

#ifndef FFMPEGDEMO_SAFE_QUEUE_H
#define FFMPEGDEMO_SAFE_QUEUE_H

#include <queue>

using std::queue;

#include <pthread.h>

template<class T>
class safe_queue {
public:
    safe_queue() {
        pthread_mutex_init(&mutex, 0);
    }

    ~safe_queue() {
        pthread_mutex_destroy(&mutex);
    }

    void push(T t);

    void pop(T &t);

private:
    queue<T> q;
    //互斥锁
    pthread_mutex_t mutex;
    //条件变量
    pthread_cond_t cond;
};


#endif //FFMPEGDEMO_SAFE_QUEUE_H
