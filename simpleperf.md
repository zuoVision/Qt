# simpleperf简介

[toc]



## 什么是simpleperf 
- simpleperf ==> simple(采样) + perf(ormance)
- simpleperf是Linux perf在Android上的替代品
- simpleperf是Android的原生CPU分析工具，用于分析运行在Android设备上的**应用程序**和**native process**（本地进程）
- 可以分析Java & c++代码
- 支持版本：>=p
- 是使用linux内核和PMU硬件支持的cpu分析工具

## simpleperf如何工作

![image-20220404143439343](C:\Users\zuozhe\AppData\Roaming\Typora\typora-user-images\image-20220404143439343.png)



### ARM PMU

现代 CPU 有一个称为性能监控单元 (PMU) 的硬件组件。 PMU 有几个硬件计数器，用于计算发生了多少 cpu 周期、执行了多少指令或发生了多少缓存未命中等事件。

### Kernel support

* kernel event driver
  * 是user space和pmu之间的桥梁
  * 把pum事件映射到perf事件
  * 提供**perf_event_open**监控选择线程的性能
* pmu driver
  * 通过 perf_pmu_register() 注册到 perf 事件驱动程序
  * cpu pmu driver ==>操作arm pmu
  * software pmu ==> cpu-clock事件等
  * tracepoint pmu ==> sched:sched_switch事件

# Excutable

### simpleperf commands

* simpleperf将功能分为如下几个字模块
  * list command:列出设备上可用的性能事件
  * stat command:监听线程，并输出性能事件计数值
  * record command：监控线程，并使用样本生成profile data(简要数据)
  * report command：将上述record 生成的profile data 生成报告数据

#### list

> list 命令列出设备上所有可用的事件。不同的设备可能支持不同的事件，因为它们具有不同的硬件和内核

```
List of hardware events:
  branch-misses
  bus-cycles
  cache-misses
  cache-references
  cpu-cycles
  instructions
  stalled-cycles-backend
  stalled-cycles-frontend
List of software events:
  alignment-faults
  context-switches
  cpu-clock
...

```

#### stat

> 获取一段时间内，要分析的进程中发生了多少事件。通过传入不同参数，我们可以选择要使用哪些事件、要监听哪些进程/线程，监听多长时间以及打印间隔

![image-20220404150135696](C:\Users\zuozhe\AppData\Roaming\Typora\typora-user-images\image-20220404150135696.png)

1. 给定用户选项，simpleperf 通过对内核进行系统调用来启用分析。
2. 内核在配置的进程运行时启用计数器。
3. 分析后，simpleperf 从内核读取计数器，并报告计数器摘要。

```
$ simpleperf stat -h
Usage: simpleperf stat [options] [command [command-args]]
       Gather performance counter information of running [command].
Options:
-p pid1,pid2,...                                                Stat events on existing processes. 	#分析进程
-t tid1,tid2,...                                                   Stat events on existing threads.		#分析线程
-a                                                                    Collect system-wide information.		#收集系统范围的信息
--cpu cpu_item1,cpu_item2,...                        Collect information only on the selected cpus. #收集选择的cup信息
-e event1[:modifier1],event2[:modifier2],...     Select a list of events to count.		#选择要统计的事件表
--duration time_in_sec                                    Monitor for time_in_sec seconds.		#选择记监听时长（s）

$ simpleperf stat -e cache-references,cache-misses -a --duration 1   
                                                                                                      
Performance counter statistics:
#           count  event_name           # count / runtime,  runtime / enabled_time
  774,728,087  cache-references   # 96.513 M/sec              (100%)
   31,985,983   cache-misses         # 4.128672% miss rate  (100%)
Total test time: 1.001893 seconds.


```

#### record

> 以采样方式生成profile data

![image-20220404150923199](C:\Users\zuozhe\AppData\Roaming\Typora\typora-user-images\image-20220404150923199.png)

```
$ simpleperf record -h
Usage: simpleperf record [options] [--] [command [command-args]]
       Gather sampling information of running [command].
Options:
-p pid1,pid2,...       Record events on existing processes.     #记录进程事件
-t tid1,tid2,...          Record events on existing threads.	#记录线程事件
-a                          System-wide collection.					#系统范围收集
--cpu cpu_item1,cpu_item2,...                       Collect information only on the selected cpus. 		#同stat
-e event1[:modifier1],event2[:modifier2],...    Select a list of events to count.					#同stat
-f freq                                                             Set event sample frequency. It means recording at
                                                                       most [freq] samples every second.   		#设置事件采样频率，意味着每秒最多采样[freq]次
--duration time_in_sec                                   Monitor for time_in_sec seconds              #同stat
-o record_file_name                                       Set record file name, default is perf.data.   #设置profile data文件名，默认perf.data
--call-graph fp | dwarf[,<dump_stack_size>]  Enable call graph recording. 			#启用调用图记录
-g                                                                    Same as '--call-graph dwarf'.

```

##### simple format

每个simple包含如下信息：
time                   - timestamp in CLOCK_MONOTONIC
pid, tid               - process id, thread id
cpu                    - cpu
period                - how many events have happened since last sample
ips[]                    - callstack (frame-pointer based call stack)
regs[]                 - userspace register values
stack[]                - user stack data up to 64k

#### report 

```
$ simpleperf report
Cmdline: /system/bin/simpleperf record -g sleep 1
Arch: arm64
Event: cpu-cycles (type 0, config 0)
Samples: 56
Event count: 13885436
Overhead  Command     Pid    Tid    Shared Object                    Symbol
9.61%         sleep       14852  14852  [kernel.kallsyms]               vma_link
8.97%         sleep       14852  14852  linker64                             soinfo_do_lookup_impl
6.42%         sleep       14852  14852  linker64                             BionicAllocator::alloc_impl
6.11%         sleep       14852  14852  [kernel.kallsyms]               __follow_mount_rcu
5.83%         sleep       14852  14852  [kernel.kallsyms]               clear_page
...
```



