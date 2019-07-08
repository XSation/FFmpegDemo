//
// Created by 薛凯 on 2019-06-28.
//

#include <safe_queue.h>


template<class T>
void safe_queue<T>::push(T t) {
    pthread_mutex_lock(&mutex);
    q.push(t);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

template<class T>
void safe_queue<T>::pop(T &t) {
    pthread_mutex_lock(&mutex);
    while (q.empty()) {
        pthread_cond_wait(&cond, &mutex);
    }
    t = q.front();
    q.pop();
    pthread_mutex_unlock(&mutex);
}