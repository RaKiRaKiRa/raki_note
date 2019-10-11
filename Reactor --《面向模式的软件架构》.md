## 作用
Reactor是一种**事件处理模式**，事件驱动应用程序使用Reactor架构来**同步地等待**一个或多个指示事件的发生，然后**分离并分派**一个或多个**服务请求**给事件处理程序中合适的**钩子（回调）方法**。
通过这种方式将**事件分离分派机制**与服务程序中与指示相关的**具体处理机制**分开。

## 结构
Reactor由5个部分组成。
1. ***句柄***：指用于识别网络连接或文件打开的事件源，由操作系统提供，在linux中一般指**文件描述符**。
2. ***同步事件分离程序***：一般指对一个函数（select， poll， epoll）的封装。用于等待句柄集发生一个或多个事件。在句柄集中没有事件发生时函数会被阻塞。
3. ***事件处理程序***：用于指定一系列接口，代表了一系列**可用操作（钩子方法，回调）的集合**，用于处理发生于相关句柄上的事件。
4. ***具体事件处理程序***：**实现**了应用程序**提供的具体服务**，每个具体事件处理程序与**一个句柄相关联**，实现了处理对应句柄的事件的钩子方法，这些事件通过相关联的句柄进行接收。
5. ***反应器***：Reactor模式的核心，用于①注册和删除事件处理程序即相应句柄的接口，②运行应用的事件处理循环（EventLoop），③有就绪事件到来时，分发事件到之前注册的回调函数上处理
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190917171817449.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1JhS2lSYUtpUmE=,size_16,color_FFFFFF,t_70)
## 处理方式
1. 应用启动，将关具体事件处理程序**注册**给反应器。
2. 运行反应器的**时间处理循环**，**调用同步事件分离程序**，**等待**指示事件在句柄集发生
3. **事件到来**，同步事件分离程序返回到反应器
4. 反应器根据就绪的句柄找到对应的事件处理程序并**分派相应的钩子方法**。
5. 钩子方法**调用服务**。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190917171803823.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1JhS2lSYUtpUmE=,size_16,color_FFFFFF,t_70)

## 例子
https://github.com/RaKiRaKiRa/Cyclone
本人完成的[网络库与HttpServer](https://github.com/RaKiRaKiRa/Cyclone) 中，完整的实现了Reactor模型
其中：
反应器 ：EventLoop
同步事件分离程序 : Epoller, Poller
事件处理程序 : Channel
具体事件处理程序 ：Acceptor， Connection
句柄 ：Socket
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190917190651874.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1JhS2lSYUtpUmE=,size_16,color_FFFFFF,t_70)

