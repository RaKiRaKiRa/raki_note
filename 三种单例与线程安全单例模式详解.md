```cpp

//Singleton返回的实例的生存期是由Singleton本身所决定的，而不是用户代码。
//我们知道，指针和引用在语法上的最大区别就是指针可以为NULL，并可以通过delete运算符删除指针所指的实例，而引用则不可以。
//由该语法区别引申出的语义区别之一就是这些实例的生存期意 义：
//通过引用所返回的实例，生存期由非用户代码管理，而通过指针返回的实例，其可能在某个时间点没有被创建，或是可以被删除的。

//重用方法：class SingletonInstance : public Singleton<SingletonInstance>

//多线程懒汉式
template <typename T> 
class Singleton    
{     
public:    
	//static函数和变量 返回引用是重点
    static T& Instance()  
    {    
    	T* tmp = m_Instance;
        if(tmp == NULL)    
        {   //用lock实现线程安全      
            pthread_mutex_lock(mutex_); 
            tmp = m_Instance;           
            if(tmp == NULL)    
            {    
                tmp = new T();    
                m_Instance = tmp;
                atexit(&Singleton::Destroy);
            }   
            pthread_mutex_unlock(mutex_)  ; 
        }    
        return *m_Instance;    
    }  
private:
    //构造 拷贝 赋值均是私有或关闭   
    Singleton() {}    
    ~Singleton(){}
	Singleton(const Singleton&)=delete;  
    Singleton& operator=(const Singleton&)=delete;  

    static void Destroy()
    {
    	if(m_Instance != NULL)
    		delete m_Instance;
    	m_Instance = NULL;
    }
	static pthread_mutex_t mutex_;  
 	static T* m_Instance; 
 	//static volatile T* m_Instance; 
};    
template <typename T> T* volatile Singleton<T>::m_Instance = 0;      
template <typename T> pthread_mutex_t Singleton<T>::mutex_ = PTHREAD_MUTEX_INITIALIZER;
  
//单线程 懒汉式
template <typename T>     
class Singleton    
{    
private:    
    Singleton()  {}  
    ~Singleton() {}   
public:    
    Singleton(const Singleton&)=delete;    
    Singleton& operator=(const Singleton&)=delete;    
    //static函数和变量 返回引用是重点  
    static T& Instance()
    {  
        static T m_Instance;    
        return m_Instance;    
    }      
};


//饿汉式  
template <typename T> 
class Singleton {  
private:   
    static T* m_instance = new Singleton();  
	Singleton() {}
	~Singleton() {}
public:  
    Singleton(const Singleton&)=delete;  
    Singleton& operator=(const Singleton&)=delete;  
    T& Instance()   
    {  
        return *m_instance;  
    }  
};  

 
```

## 保护默认函数
Singleton限制其类型实例有且只能有一个，因此我们应通过将构造函数设置为非公有来保证其不会被用户代码随意创建，中间件代码需要非常严谨才能防止用户代码的误用。
在类型实例访问函数中，我们通过局部静态变量（懒汉）/成员静态变量（饥汉）达到实例仅有一个的要求。
	**要保护的有构造函数，拷贝构造函数，析构函数以及赋值运算符。**
**Singleton所返回的常常是一个引用**，对引用进行取址将得到相应类型的指针。而从语法上来说，**引用和指针的最大区别在于是否可以被delete关键字删除以及是否可以为NULL**。但是Singleton返回一个引用也 就表示其生存期由非用户代码所管理。因此使用取址运算符获得指针后又用delete关键字删除Singleton所返回的实例明显是一个用户错误。综上所述，通过将取址运算符设置为私有没有多少意义。


## 饥汉与懒汉？生存期管理
对Singleton的生存期特性的讨论需要分为两个方面：**Singleton内使用的静态变量的生存期**以及 **Singleton外在用户代码中所表现的生存期**。
**懒汉**Singleton内使用的静态变量是一个**局部静态变量**，因此只有在Singleton的 **Instance()函数被调用时其才会被创建**，从而拥有了**延迟初始化**（Lazy）的效果，**提高了程序的启动性能**。同时该实例将生存至程序执行完毕。而就 Singleton的用户代码而言，其生存期贯穿于整个程序生命周期，从程序启动开始直到程序执行完毕。当然，**懒汉Singleton在生存期上的一个缺陷就是创建和析构时的不确定性。**由于Singleton实例会在Instance()函数被访问时被创建，因此**在某处新添加的一处对Singleton的访问将可能导致Singleton的生存期发生变化**。如果其依赖于其它组成，如另一个Singleton，那么对它们的生存期进行管理将成为一个灾难。甚至可 以说，还不如不用Singleton，而使用明确的实例生存期管理。程序初始化及关闭时单件的构造及析构顺序的不确定可能导致致命的错误
将Singleton的实现改为**使用全局静态变量，称为饿汉Singleton**，并将这些全局静态变量在文件中**按照特定顺序排序**，但是这样的话，静态变量将使用eager initialization的方式完成初始化，**可能会对性能影响较大。但优点是线程安全性**。对于具有关联的两个Singleton，对它们进行使用的代码常常局限在同一区域内。该问题的一个解决方法常常是将对它们进行使用的管理逻辑实现为Singleton，而在内部逻辑中对它们进行明确的生存期管理。
全局静态变量的生命周期：编译器会在程序的main()函数执行之前插入一段代码，用来初始化全局变量。当然，静态变量也包含在内。该过程被称为静态初始化。

## 多线程懒汉知识点
使用了一个指针记录创建的Singleton实例. 为了能满足局部静态变量只被初始化一次的需求，**很多编译器会通过一个全局的标志位记录该静态变量是否已经被初始化的信息**。那么，对静态变量进行初始化的伪码就变成下面这个样子：
```cpp
1 bool flag = false;
2 if (!flag)
3 {
4     flag = true;
5     staticVar = initStatic();
6 }
```
在第一个线程执行完对flag的检查并进入if分支后，**第二个线程将可能被启动，从而也进入if分支。这样，两个线程都将执行对静态变量 的初始化。** 因此在这里，我使用了指针，并在对指针进行赋值之前使用锁保证在**同一时间内只能有一个线程对指针进行初始化**。同时基于性能的考虑，我们需要在**每次访问实例之前检查指针是否已经经过初始化**，以避免每次对Singleton的访问都需要请求对锁的控制权。
同时因为new运算符的调用分为分配内存、调用构造函数以及为指针赋值三步，就像下面的构造函数调用：
```cpp
SingletonInstance pInstance = new SingletonInstance();
```
会转化为以下形式：
```cpp
SingletonInstance pHeap = __new(sizeof(SingletonInstance));
pHeap->SingletonInstance::SingletonInstance();
SingletonInstance pInstance = pHeap;
```
这样转换是因为在C++标准中规定，**如果内存分配失败，或者构造函数没有成功执行， new运算符所返回的将是空**。一般情况下，编译器不会轻易调整这三步的执行顺序，但是在满足特定条件时，如构造函数不会抛出异常等，编译器可能出于优化的 目的将第一步和第三步合并为同一步,可以理解为二三步互换：
```cpp
SingletonInstance pInstance = __new(sizeof(SingletonInstance));
pInstance->SingletonInstance::SingletonInstance();
```
这样就可能导致其中一个线程在完成了内存分配后就被切换到另一线程，而另一线程对Singleton的**再次访问将由于pInstance已经 赋值而越过if分支，从而返回一个不完整的对象。**因此，我在这个实现中为静态成员指针添加了volatile关键字**该关键字的实际意义是由其修饰的变量可能会被意想不到地改变，因此每次对其所修饰的变量进行操作都需要从内存中取得它的实际值。** 它可以用来阻止编译器对指令顺序的调整。只是由于该关键字所提 、供的禁止重排代码是假定在单线程环境下的，因此并不能禁止多线程环境下的指令重排。
		或者用临时指着`T* tmp`人工模拟将两步扩展成三步。
	**最后来说说对atexit()关键字的使用。** 在通过new关键字创建类型实例的时候，我们同时通过atexit()函数注册了释放该实例的函数，从而保证了这些实例能够在 **程序退出前正确顺序地析构** 。该函数的特性也能  **保证后被创建的实例首先被析构** 。 其实，对静态类型实例进行析构的过程与前面所提到 的在main()函数执行之前插入静态初始化逻辑相对应。


## 指针还是引用
因为Singleton返回的**实例的生存期是由Singleton本身所决定的**，而不是用户代码。我们知道，**指针和引用在语法上的最大区别就是指针可以为NULL**，并可以通过delete运算符**删除指针所指的实例**，**而引用则不可以**。由该语法区别引申出的语义区别之一就是这些实例的生存期意义：通过引用所返回的实例，生存期由非用户代码管理，而通过指针返回的实例，其可能在某个时间点没有被创建，或是可以被删除的。但是这两条 Singleton都不满足，因此在这里，我使用指针，而不是引用。

**指针与引用其他区别：** 低层次向高层次上来说，分为编译器实现上的，语法上的以及语义上的区别。就编译器的实现来说， 声**明一个引用并没有为引用分配内存，而仅仅是为该变量赋予了一个别名。** 而声明一个指针则分配了内存。这种实现上的差异就导致了语法上的众多区别：对引用进 行更改将导致其原本指向的实例被赋值，而对指针进行更改将导致其指向另一个实例；**引用将永远指向一个类型实例，从而导致其不能为NULL** ，并由于该限制而 导致了众多语法上的区别，如 **dynamic_cast对引用和指针在无法成功进行转化时的行为不一致**。而就语义而言，前面所提到的生存期语义是一个区别， 同时一个返回引用的函数常常保证其返回结果有效。一般来说，语义区别的根源常常是语法上的区别，因此上面的语义区别仅仅是列举了一些例子，而真正语义上的 差别常常需要考虑它们的语境。

