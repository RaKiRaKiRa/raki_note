/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-14 00:01
 * Last modified : 2019-12-14 00:34
 * Filename      : 
 * Description   : 
 **********************************************************/
#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#include <deque>
#include <pthread.h>
template<typename T>
class BlockingQueue
{
public:
    BlockingQueue(int capacity):
        queue_(),
        capacity_(capacity),
        size_(0)
    {
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&isFull_, NULL);
        pthread_cond_init(&isEmpty_, NULL);
    }

    BlockingQueue(BlockingQueue& bq) = delete;

    BlockingQueue& operator=(BlockingQueue& bq) = delete;

    int size() const
    {
        return size_;
    }

    int capacity() const
    {
        return capacity_;
    }

    void setCapacity(int capacity)
    {
        pthread_mutex_lock(&mutex_);
        capacity_ = capacity;
        pthread_mutex_unlock(&mutex_);
    }

    void push(const T& t)
    {
        pthread_mutex_lock(&mutex_);
        while(capacity_ != -1 && size_ >= capacity_)
        {
            pthread_cond_wait(&isFull, &mutex_);
        }
        queue_.push_back(t);
        ++size_;
        pthread_cond_broadcast(&isEmpty);
        pthread_mutex_unlock(&mutex_);
    }

    T pop()
    {
        pthread_mutex_lock(&mutex_);
        while(size_ == 0)
        {
            pthread_cond_wait(&isEmpty);
        }
        T res = queue_.front();
        queue_.pop_front();
        --size_;
        pthread_cond_broadcast(&isFull);
        pthread_mutex_unlock(&mutex_);
    }

    bool isEmpty() const
    {
        bool res;
        pthread_mutex_lock(&mutex_);
        res = (size_ == 0);
        pthread_mutex_unlock(&mutex_);
        return res;
    }

private:
    std::deque<T>   queue_;
    int             capacity_;
    int             size_;
    pthread_mutex_t mutex_;
    pthread_cond_t  isFull_;
    pthread_cond_t  isEmpty_;
};

#endif