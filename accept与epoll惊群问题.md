## 什么是惊群

惊群现象（thundering herd）就是当**多个进程和线程在同时阻塞等待同一个事件**时，如果这个事件发生，会唤醒所有的进程，但最终只可能有一个进程/线程对该事件进行处理，其他进程/线程会在失败后重新休眠，每次唤醒与休眠都存在上下文切换，这种性能浪费就是惊群。

## accept惊群

主进程创建socket, bind, **listen之后，fork出多个子进程**，每个子进程都开始循环处理（accept)这个socket。每个进程都阻塞在accpet上，当一个新的连接到来时，**所有的进程都会被唤醒，但其中只有一个进程会accept成功**，其余皆失败，重新休眠。这就是accept惊群。

*那么这个问题真的存在吗？*

事实上，历史上，Linux 的 accpet 确实存在惊群问题，但**现在的内核都解决该问题了**。即，当多个进程/线程都阻塞在对同一个 socket 的 accept 调用上时，当有一个新的连接到来，**内核只会唤醒一个进程**，其他进程保持休眠，压根就不会被唤醒。

## epoll惊群

如上所述，accept 已经不存在惊群问题，但 epoll 上还是存在惊群问题。即，如果**多个阻塞在 epoll_wait 的进程/线程，都监听了同一个 listening socket fd **，当有一个新的连接到来时，所有的进程都会被唤醒。

主进程创建 socket, bind， listen 后，**将该 socket 加入到 epoll 中，然后 fork 出多个子进程，每个进程都阻塞在 epoll_wait 上**，如果有事件到来，则判断该事件是否是该 socket 上的事件，如果是，说明有新的连接到来了，则进行 accept 操作。为了简化处理，忽略后续的读写以及对 accept 返回的新的套接字的处理，直接断开连接。

那么，当新的连接到来时，是否每个阻塞在 epoll_wait 上的进程都会被唤醒呢？

很多博客中提到，测试表明虽然 epoll_wait **不会像 accept 那样只唤醒一个进程/线程，但也不会把所有的进程/线程都唤醒**。而事实上，**其余进程没有被唤醒的原因是你的某个进程已经处理完这个 accept，内核队列上已经没有这个事件，无需唤醒其他进程**。你可以在 epoll 获知这个 accept 事件的时候，不要立即去处理，而是 sleep 下，这样所有的进程都会被唤起。

所以，epoll_wait上的惊群确实是存在的，**所有的进程都会被唤醒**。

## 为什么内核不处理epoll惊群

我认为：

accept 确实应该只能被一个进程调用成功，内核很清楚这一点。但 epoll 不一样，他监听的文件描述符，除了可能后续被 accept 调用外，还有可能是其他网络 IO 事件的，而其他 **IO 事件是否只能由一个进程处理，是不一定的**，内核不能保证这一点，这是一个由用户决定的事情，**例如可能一个文件会由多个进程来读写**。所以，对 epoll 的惊群，内核则不予处理。

## Nginx 是如何处理惊群问题的

在思考这个问题之前，我们应该以前对前面所讲几点有所了解，即先弄清楚问题的背景，并能自己复现出来，而不仅仅只是看书或博客，然后再来看看 Nginx 的解决之道。这个顺序不应该颠倒。

首先，我们先大概梳理一下 Nginx 的网络架构，几个关键步骤为:

1. Nginx 主进程解析配置文件，根据 listen 指令，将监听套接字初始化到全局变量 ngx_cycle 的 listening 数组之中。此时，监听套接字的创建、绑定工作早已完成。
2. Nginx 主进程 fork 出多个子进程。
3. 每个子进程在 ngx_worker_process_init 方法里依次调用各个 Nginx 模块的 init_process 钩子，其中当然也包括 NGX_EVENT_MODULE 类型的 ngx_event_core_module 模块，其 init_process 钩子为 ngx_event_process_init。
4. ngx_event_process_init 函数会初始化 Nginx 内部的连接池，并把 ngx_cycle 里的监听套接字数组通过连接池来获得相应的表示连接的 ngx_connection_t 数据结构，这里关于 Nginx 的连接池先略过。我们主要看 ngx_event_process_init 函数所做的另一个工作：如果在配置文件里**没有**开启[accept_mutex锁](http://nginx.org/en/docs/ngx_core_module.html#accept_mutex)，就通过 ngx_add_event 将所有的监听套接字添加到 epoll 中。
5. 每一个 Nginx 子进程在执行完 ngx_worker_process_init 后，会在一个死循环中执行 ngx_process_events_and_timers，这就进入到事件处理的核心逻辑了。
6. 在 ngx_process_events_and_timers 中，如果在配置文件里开启了 accept_mutext 锁，子进程就会去获取 accet_mutext 锁。如果获取成功，则通过 ngx_enable_accept_events 将监听套接字添加到 epoll 中，否则，不会将监听套接字添加到 epoll 中，甚至有可能会调用 ngx_disable_accept_events 将监听套接字从 epoll 中删除（如果在之前的连接中，本worker子进程已经获得过accept_mutex锁)。
7. ngx_process_events_and_timers 继续调用 ngx_process_events，在这个函数里面阻塞调用 epoll_wait。

至此，关于 Nginx 如何处理 fork 后的监听套接字，我们已经差不多理清楚了，当然还有一些细节略过了，比如在每个 Nginx 在获取 accept_mutex 锁前，还会根据当前负载来判断是否参与 accept_mutex 锁的争夺。

把这个过程理清了之后，Nginx 解决惊群问题的方法也就出来了，就是**利用 accept_mutex 这把锁**。

如果配置文件中没有开启 accept_mutex，则所有的监听套接字不管三七二十一，都加入到每子个进程的 epoll中，这样当一个新的连接来到时，所有的 worker 子进程都会惊醒。

如果配置文件中开启了 accept_mutex，则**只有一个子进程会将监听套接字添加到 epoll 中**，这样当一个新的连接来到时，当然就只有一个 worker 子进程会被唤醒了。

## 总结

- **accept 不会有惊群，epoll_wait 才会。**
- Nginx 的 accept_mutex,并不是解决 accept 惊群问题，而是解决 epoll_wait 惊群问题。
- 说Nginx 解决了 epoll_wait 惊群问题，也是不对的，**它只是控制是否将监听套接字加入到epoll 中**。监听套接字只在一个子进程的 epoll 中，当新的连接来到时，其他子进程当然不会惊醒了。