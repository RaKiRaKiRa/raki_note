/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-14 00:22
 * Last modified : 2019-12-14 00:59
 * Filename      : 
 * Description   : 
 **********************************************************/
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <deque>
#include <functional>
#include <vector>

class ThreadPool
{
public:
    ThreadPool(int threadNum):
        taskList_(),
        threadList_(threadNum),
        threadNum_(threadNum)
    {
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&isEmpty_, NULL);
    }


    void start()
    {
        for(int i = 0; i < threadNum_; ++i)
        {
            if(!pthread_create(&threadList_[i], NULL, threadRun, (void*)this))
            {
            	printf("error");
            }
        }
    }

    //线程里实际执行的函数
    void run()
    {
        while(1)
        {
            pthread_mutex_lock(&mutex_);
            while(taskList_.empty())
            {
                pthread_cond_wait(&isEmpty_, &mutex_);
            }
            std::function<void()> func = taskList_.front();
            taskList_.pop_front();
            pthread_mutex_unlock(&mutex_);
            func();
        }
    }

    // 重点！！！ 跳板函数
    // 静态：pthread_create需要的参数类型为void* (*)(void*)，而run作为类的成员函数时其类型是void (Threadpool::)(void*)的成员函数指针。所以需要一个void* (*)(void*)类型的跳板
    // 返回void*
    static void* threadRun(void* arg)
    {
        ThreadPool* tp = (ThreadPool*)arg;
        tp->run();
        return NULL;
    }

    void addTask(std::function<void()> cb)
    {
        pthread_mutex_lock(&mutex_);
        taskList_.push_back(cb);
        pthread_cond_broadcast(&isEmpty_);
        pthread_mutex_unlock(&mutex_);
    }
private:
    std::deque<std::function<void()>> taskList_;
    std::vector<pthread_t> threadList_;
    int threadNum_;
    pthread_mutex_t mutex_;
    pthread_cond_t  isEmpty_;
};


#endif