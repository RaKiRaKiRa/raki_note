#pragma once
#ifndef MYSHAREDPTR_H
#define MYSHAREDPTR_H
#include <atomic>

//坑1：++count_和--count_前应该先判断obj_是否为空
//坑2：赋值前应判断count_ != &rhs
//坑3：reset时应判断count_ != &rhs和obj_是否为空

template<typename T>
class SharedPtr
{
	friend class SharedPtr<T>;
public:
	SharedPtr() :
		count_(NULL),
		obj_(NULL)
	{}

	SharedPtr(T* obj) :
		count_(new std::atomic<int>(1)),
		obj_(obj)
	{}

	SharedPtr(SharedPtr& ptr) :
		count_(ptr.count_),
		obj_(ptr.get())
	{
		if (obj_ != NULL)
		{
			++(*count_);
		}
	}
	SharedPtr(SharedPtr&& ptr) :
		count_(ptr.count_),
		obj_(ptr.get())
	{
		ptr.count_ = NULL;
		ptr.obj_ = NULL;
	}

	SharedPtr& operator=(SharedPtr& rhs)
	{
		if (this != &rhs)
		{
			if (obj_ != NULL)
			{
				--(*count_);
				if ((*count_) == 0)
				{
					delete obj_;
					delete count_;
				}
			}
			count_ = rhs.count_;
			obj_ = rhs.get();
			if(obj_ != NULL)
				++(*count_);
		}

		return *this;
	}
	SharedPtr& operator=(SharedPtr&& rhs)
	{
		if (this != &rhs)
		{
			count_ = rhs.count_;
			obj_ = rhs.get();
			rhs.count_ = NULL;
			rhs.obj_ = NULL;
		}
		return *this;
	}

	// 重点！！！
	~SharedPtr()
	{
		if (obj_ != NULL)
		{
			--(*count_);
			if ((*count_) == 0)
			{
				delete obj_;
				delete count_;
			}
		}
	}

	
	T& operator*()
	{
		return *obj_;
	}
	
	T* operator->()
	{
		return obj_;
	}

	int count() const
	{
		if (obj_ == NULL)
			return 0;
		return *count_;
	}

	T* get() const
	{
		return obj_;
	}

	void reset()
	{
		if (obj_ == NULL)
			return;
		if (obj_ != NULL)
		{
			--(*count_);
			if ((*count_) == 0)
			{
				delete obj_;
				delete count_;
			}
		}
		count_ = NULL;
		obj_ = NULL;
	}
	void reset(T* obj)
	{
		if (obj_ != NULL)
		{
			--(*count_);
			if ((*count_) == 0)
			{
				delete obj_;
				delete count_;
			}
		}
		obj_ = obj;
		if (obj_ != NULL)
			count_ = new std::atomic<int>(1);
		else
			count_ = NULL;
	}
	void reset(const SharedPtr& rhs)
	{
		if (obj_ == NULL)
			return;
		if (this != &rhs)
		{
			if (obj_ != NULL)
			{
				--(*count_);
				if ((*count_) == 0)
				{
					delete obj_;
					delete count_;
				}
			}
			count_ = rhs.count_;
			obj_ = rhs.get();
			if(obj_ != NULL)
				++(*count);
		}
	}
private:
	std::atomic<int>* count_;
	T* obj_;
};

#endif