/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-13 00:18
 * Last modified : 2019-12-13 23:58
 * Filename      : 
 * Description   : 
 **********************************************************/
#ifndef SINGLETON_H
#define SINGLETON_H
#include <pthread.h>

template<typename T>
class Singleton
{
private:
    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton& rhs) = delete;
    Singleton& operator=(const Singleton& rhs) = delete;
public:

    T& Instance()
    {
        T* tmp = instance_;
        if(tmp == NULL)
        {
            pthread_mutex_lock(&mutex_);
            tmp = instance_;
            if(tmp == NULL)
            {   
                //防止刚分配完空间就有其他线程访问，越过if
                tmp = new T();
                instance_ = tmp;
            }
            pthread_mutex_unlock(&mutex_);
        }
        return *instance_;
    }

private:
    static T* instance_;
    static pthread_mutex_t mutex_;
};

template<typename T>
T* Singleton<T>::instance_ = NULL;

template<typename T>
pthread_mutex_t Singleton<T>::mutex_ = PTHREAD_MUTEX_INITIALIZER;

#endif