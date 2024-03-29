### 简述

​	Linux 从整体上区分**实时进程和普通进程**，因为实时进程和普通进程度调度是不同的，它们两者之间，实时进程应该先于普通进程而运行，然后，对于同一类型的不同进程，采用不同的标准来选择进程。**对普通进程**的调度策略是**完全公平调度（CFS）**，对于实时进程采用了两种调度策略**，FIFO(先来先服务调度)和RR（时间片轮转调度）**。

### FIFO(先来先服务调度)

​	先来先服务(FCFS)调度算法是一种最简单的调度算法，该算法既可用于作业调度，也可用于进程调度。当在作业调度中采用该算法时，每次调度都是从后备作业队列中选择一个或多个最先进入该队列的作业，将它们调入内存，为它们分配资源、创建进程，然后放入就绪队列。在进程调度中采用FCFS算法时，则**每次调度是从就绪队列中选择一个最先进入该队列的进程**，为之分配处理机，使之投入运行。该进程**一直运行到完成或发生某事件而阻塞**后才放弃处理机。

### RR（时间片轮转调度）

在早期的时间片轮转法中，系统将所有的就绪进程按先来先服务的原则排成一个队列，每次调度时，把CPU分配给队首进程，并令其执行一个时间片。时间片的大小从几ms到几百ms。当执行的时间片用完时，**由一个计时器发出时钟中断请求，调度程序便据此信号来停止该进程的执行，并将它送往就绪队列的末尾；然后，再把处理机分配给就绪队列中新的队首进程，同时也让它执行一个时间片**。这样就可以保证就绪队列中的所有进程在一给定的时间内均能获得一时间片的处理机执行时间。换言之，系统能在给定的时间内响应所有用户的请求。



### CFS（完全公平调度）

**完全公平调度算法（CFS，SCHED_NORMAL）**用于普通进程的调度

​	在这种调度中，最理想状态是有n个进程则每个进程都能获得1/n的处理器时间，然而我们并不能真正在处理器上同时运行n个进程，所以**每个进程都按其权重（nice）在全部可运行的进程中（nice的总和）所占的比例来运行**。任何进程所获得的处理器时间是由他自己和其他所有可运行进程nice值的相对差来决定的。
​	CFS的思想就是<font color=red>**让每个调度实体的vruntime互相追赶**</font>，而每个调度实体的vruntime增加速度不同，权重越大的增加的越慢，这样就能获得更多的cpu执行时间。

​	**谁的vruntime值较小就说明它以前占用cpu的时间较短**，受到了“不公平”对待，因此下一个运行进程就是它。**这样既能公平选择进程，又能保证高优先级进程获得较多的运行时间。**

#### **①时间记账：**

​	CFS使用调度器实体结构（sched_entity）来追踪进程运行进账，它作为struct task_struct的se成员。
​    	sched_entity中有成员vruntime，存放进程的虚拟运行时间，这个时间是将该进程花在运行上的时间和根据所有可运行进程数标准化后得到。**用于记录一个程序到底运行了多久以及它还应该再运行多久**。系统会周期性调用update_curr()，**计算当前进程的实际执行时间**，然后**根据其权重加权**后加到vruntime。

​    	<font color=red>**vruntime += 实际运行时间 \* 1024 / 进程权重 ** </font>（1024是nice=0的权重）

更详细：

| 条件                   | 公式                                       |
| ---------------------- | ------------------------------------------ |
| curr.nice!=NICE_0_LOAD | vruntime += delta * NICE_0_LOAD/se.weight; |
| curr.nice=NICE_0_LOAD  | vruntime += delta;                         |



**②进程选择**：

​    	**<font color=red>CFS会选择最小vruntime的进程作为下一个运行进程</font>**，所有的可运行进程被置于一个红黑树上（可执行队列），其键值就是vruntime。

​    ​	当进程被唤醒或者通过fork调用第一次创建进程时，就会加入可执行队列；

​	而进程被阻塞或终止时，就会从可执行队列中删除。

#### **③调度器：**

​    ​	进程调度的入口是schedule()，他会询问优先级最高的调度类，**从其可运行队列中获得vruntime最小**的进程。

#### **④睡眠与唤醒：**

​    	**睡眠**：进程将自己标记为休眠，从可执行队列中移除，置于等待队列（每个队列都有），然后调用schedule()选择与调度下一进程；
​    ​	**唤醒**：将等待队列的进程标记为可运行，然后将此进程放入可运行队列（每个处理器一个），且如果该进程优先级比正在执行的进程要高，则设置need_resched，执行用户抢占；

#### **⑤抢占：**

​    ​	内核提供一个标志位need_resched表明是否需要重新调度，有两种情况会设置need_resched：

​    ​			优先级高的进程进入可执行状态时
​    ​			某个进程应该被抢占时

​    ​	*用户抢占：*
​    		当内核

​    ​			**从系统调用返回用户空间时**

​    ​			**或从终端处理程序返回用户空间时**，

​    ​		会检查need_resched。
​    ​	*内核抢占：*

​    ​		只要重新调度是安全的（**当前进程没有持有锁**），内核就可以在任何时间抢占正在执行的。

​    ​		内核抢占发生在：

​    ​			**中断处理程序正在进行，且返回内核空间前，**

​    ​			**内核代码再一次具有可抢占性时，**

​    ​			**如果内核中的任务显式调用schedule，**

​    			**如果内核中的如何阻塞**

#### ⑥多处理器

​	CFS为每个处理器都单独配备一个运行队列，每个队列都有自己的锁，本质上就是要在调度程序中每次都把整个调度程序下方到单个处理器执行，增加了可扩展性。

### 总结CFS

#### CFS调度算法的思想

理想状态下每个进程都能获得相同的时间片，并且同时运行在CPU上，但实际上一个CPU同一时刻运行的进程只能有一个。也就是说，当一个进程占用CPU时，其他进程就必须等待。CFS为了实现公平，必须惩罚当前正在运行的进程，以使那些正在等待的进程下次被调度.

#### 虚拟时钟是红黑树排序的依据

具体实现时，CFS通过每个进程的虚拟运行时间(vruntime)来衡量哪个进程最值得被调度. CFS中的就绪队列是一棵以vruntime为键值的红黑树，虚拟时间越小的进程越靠近整个红黑树的最左端。因此，调度器每次选择位于红黑树最左端的那个进程，该进程的vruntime最小.

#### 优先级计算负荷权重, 负荷权重和当前时间计算出虚拟运行时间

虚拟运行时间是通过进程的实际运行时间和进程的权重(weight)计算出来的。在CFS调度器中，将进程优先级这个概念弱化，而是强调进程的权重。一个进程的权重越大，则说明这个进程更需要运行，因此它的虚拟运行时间就越小，这样被调度的机会就越大。而，CFS调度器中的权重在内核是对用户态进程的优先级nice值, 通过prio_to_weight数组进行nice值和权重的转换而计算出来的

