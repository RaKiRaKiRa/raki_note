/**********************************************************
 * Author        : RaKiRaKiRa
 * Email         : 763600693@qq.com
 * Create time   : 2019-12-13 01:17
 * Last modified : 2019-12-13 23:57
 * Filename      : 
 * Description   : 
 **********************************************************/
#ifndef MYSHAREDPTR_H
#define MYSHAREDPTR_H
#include <atomic>

template<typename T>
class mySharedPtr
{
	friend class mySharedPtr<T>;
public:
    mySharedPtr(): 
        count_(NULL), 
        obj_(NULL)
    {}

    mySharedPtr(T* obj):
        count_(new std::atomic<int>(1)),
        obj_(obj)
    {}

    mySharedPtr(mySharedPtr& ptr):
        count_(ptr.count_),
        obj_(ptr.get())
    {
        ++(*count_);
    }

    mySharedPtr& operator=(const mySharedPtr& rhs)
    {
        --(*count_);
        if((*count_) == 0)
        {
        	delete obj_;
        }
        count_ = rhs.count_;
        obj_   = rhs.get();
        ++(*count);
    }

    // 重点！！！
    ~mySharedPtr()
    {
        --(*count_);
        if(*count_ == 0)
        {
            delete count_;
            delete obj_;
        }
    }

    // 重点！！！
    T& operator*()
    {
        return *obj_;
    }
    // 重点！！！
    T* operator->()
    {
        return obj_;
    }

    int count() const
    {
        return *count_;
    }

    T* get() const
    {
        return obj_;
    }

    void reset()
    {
    	 --(*count_);
        if(*count_ == 0)
        {
            delete count_;
            delete obj_;
        }
        count_ = NULL;
        obj_   = NULL;
    }
    void reset(T* obj)
    {
    	 --(*count_);
        if(*count_ == 0)
        {
            delete count_;
            delete obj_;
        }
        count_ = new std::atomic<int>(1);
        obj_   = obj;
    }
private: 
    std::atomic<int>* count_;
    T* obj_;
};

#endif