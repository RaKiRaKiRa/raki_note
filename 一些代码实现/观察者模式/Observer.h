/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-13 00:38
 * Last modified : 2019-12-15 00:20
 * Filename      : 
 * Description   : 
 **********************************************************/
#ifndef OBSERVER_H
#define OBSERVER_H
#include <pthread.h>
#include <memory>
#include <list>
#include <algorithm>
class Observer
{
public:
    virtual ~Observer();
    virtual void update() = 0;

};

class Observed
{
public:
    Observed()
    {
        pthread_mutex_init(&mutex_, NULL);
        ObserverList_ = std::make_shared<std::list<std::weak_ptr<Observer> > >(new std::list<std::weak_ptr<Observer> >());
    }

    ~Observed()
    {
        pthread_mutex_destroy(&mutex_);
    }

    void regist(std::weak_ptr<Observer> observer)
    {
        pthread_mutex_lock(&mutex_);
        if(!ObserverList_.unique()) // 有在读，则写时复制（创建新的用于写，旧的用于读，读完就删）
        {
            ObserverList_.reset(new std::list<std::weak_ptr<Observer> >(*ObserverList_));
        }
        ObserverList_->push_back(observer);
        pthread_mutex_unlock(&mutex_);
    }

    void unregist(std::weak_ptr<Observer> observer)
    {
        pthread_mutex_lock(&mutex_);
        if(!ObserverList_.unique()) // 有在读，则写时复制（创建新的用于写，旧的用于读，读完就删）
        {
            ObserverList_.reset(new std::list<std::weak_ptr<Observer> >(*ObserverList_));
        }
        //找到需注销的指针
        std::list<std::weak_ptr<Observer> >::iterator it = std::find(ObserverList_->begin(), ObserverList_->end(), observer);
        if(it != ObserverList_->end())
            ObserverList_->erase(it);
        pthread_mutex_unlock(&mutex_);
    }

    void notifyAll()
    {
        //引用计数加1， 使得regist的时候启用新list
        std::shared_ptr<std::list<std::weak_ptr<Observer> > > ObserverList;
        pthread_mutex_lock(&mutex_);
        ObserverList = ObserverList_;
        pthread_mutex_unlock(&mutex_);
        std::list<std::weak_ptr<Observer> >::iterator it = ObserverList->begin();
        while(it != ObserverList->end())
        {
            std::shared_ptr<Observer> sit(it->lock());
            // 存在则通知，不存在则删除
            if(sit)
            {
                sit->update();
                ++it;
            }
            else
            {
                //it = ObserverList->erase(it);
                unregist(*it++);
            }
        }
    }
private:
    pthread_mutex_t mutex_;
    std::shared_ptr<std::list<std::weak_ptr<Observer> > > ObserverList_; //队列用list存储观察者指针
};

#endif