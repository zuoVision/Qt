# One Click软件
@[toc]

## 软件功能
* adb常用命令一键执行，如获取android设备号（adb devices）、拍照、截屏等
* Linux常用命令一键执行（待开发）
* Git常用命令一键执行（待开发）
* simpleperf(Android性能分析工具)一键执行，并生成火焰图
* XTS测试一键执行，测试结果解析，并针对fail项给出历史解决方案供参考

## 开发工具
* Qt5

## 支持平台
* Linux（推荐）
* Windows(待打包)

## 软件实现原理
运用QThread启动多线程，实现主线程（界面）和子线程（processor）分离，运用QProcess (The QProcess class is used to start external programs and to communicate with them)启动/bin/bash脚本，在UI界面输入命令或点击按钮将命令通过发送信号（emit signal）的形式传递至processor，然后由processor启动bash执行命令，并将执行结果、状态、output同样已信号的方式返回至UI，显示在textEdit。
![在这里插入图片描述](https://img-blog.csdnimg.cn/3e755530eafe49c9809e86324f4fd63f.png?x-oss-process=image)
## CommonCommand
CommonCommand模块界面包含3个部分
* 命令输入栏
* 功能按钮区
* 输出栏

![在这里插入图片描述](https://img-blog.csdnimg.cn/ca2b8d19802c475ab50d6aa9f1253b3c.png?x-oss-process=image)
#### 命令输入栏
手动输入执行命令，通常用于自定义带参命令的执行。
输入栏自动补全功能如下：
![在这里插入图片描述](https://img-blog.csdnimg.cn/8308e965a69143868b6408777602141b.gif#pic_center)
> save command：保存当前输入的命令，以将该命令更新至补全词库(native_cmd_list.txt)
> complete regular：补全模式选择

#### 功能按钮区
目前已实现的功能有：adb device，root，remount，手机截屏，拍照等
示例：截屏
通过下面输出栏可以看到，这个按钮点击事件其实是在执行：adb shell screencap -p /storage/img_20220420171252.png;adb pull /storage/img_20220420171252.png capture（将图片pull到./capture）,另外可以看到在执行命令时，最下方状态栏可以看到有绿灯闪烁，表示命令正在执行过程中，执行结束，灯灭。
![在这里插入图片描述](https://img-blog.csdnimg.cn/61f07166e0e746238c39e472cafafbd1.gif#pic_center)
#### 输出栏
执行命令实时回显，如上图。类似shell终端，将输出结果实时显示在textEdit控件。


## simpleperf
simpleperf = simple + perf
simple:不是简单的意思，而是**采样**
perf:performance--性能

![在这里插入图片描述](https://img-blog.csdnimg.cn/5ca8eb7c6c474212a6b02ca80a5a4637.png)
### list
list 命令列出设备上所有可用的事件。不同的设备可能支持不同的事件，因为它们具有不同的硬件和内核
> 可以看到输出栏可以拖拽出来方便查看，可双击也可直接拖拽

![在这里插入图片描述](https://img-blog.csdnimg.cn/7ec2ca1ee1bd43b69bef3bd2efc16cb0.gif#pic_center)
### stat
获取一段时间内，要分析的进程中发生了多少事件。通过传入不同参数，我们可以选择要使用哪些事件、要监听哪些进程/线程，监听多长时间以及打印间隔,当前可支持客制化的参数如图所示，更详细的参数参见‘simpleperf stat -h’，当然你也可以直接在Command中的命令行执行自己客制化的参数

![在这里插入图片描述](https://img-blog.csdnimg.cn/1a3c3de996a44fb48f84413480573596.png)

```shell
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
> 默认分析命令： adb shell system/bin/simpleperf stat -e cache-references,cache-misses -a --duration 10 

![在这里插入图片描述](https://img-blog.csdnimg.cn/20423800342a47569c81aaf62c2453f3.gif#pic_center)

### record
以采样方式生成profile data，当前可支持客制化的参数如图所示，更详细的参数参见‘simpleperf stat -h’，当然你也可以直接在Command中的命令行执行自己客制化的参数

![在这里插入图片描述](https://img-blog.csdnimg.cn/93fd3d1e053e493e8346fa5fb93963da.png)

> 默认执行命令：adb shell rm /data/local/tmp/perf.data;adb shell system/bin/simpleperf record -o /data/local/tmp/perf.data -e task-clock:u -f 1000 -g --duration 10 --log info --app com.tcl.camera;adb pull /data/local/tmp/perf.data
> 记录相机app10s内的性能数据，并pull到本地
```shell
$ simpleperf record -h
Usage: simpleperf record [options] [--] [command [command-args]]
       Gather sampling information of running [command].
Options:
-p pid1,pid2,...         Record events on existing processes.     #记录进程事件
-t tid1,tid2,...         Record events on existing threads.	#记录线程事件
-a                       System-wide collection.					#系统范围收集
--cpu cpu_item1,cpu_item2,...    Collect information only on the selected cpus. 		#同stat
-e event1[:modifier1],event2[:modifier2],...    Select a list of events to count.					#同stat
-f freq                 Set event sample frequency. It means recording at most [freq] samples every second.   		#设置事件采样频率，意味着每秒最多采样[freq]次
--duration time_in_sec  Monitor for time_in_sec seconds              #同stat
-o record_file_name     Set record file name, default is perf.data.   #设置profile data文件名，默认perf.data
--call-graph fp | dwarf[,<dump_stack_size>]  Enable call graph recording. 			#启用调用图记录
-g                   Same as '--call-graph dwarf'.
```
![在这里插入图片描述](https://img-blog.csdnimg.cn/bcfd0f83e02240d68193700f0829d1ed.gif#pic_center)

### report
生成数据报告
> 默认执行命令：python scripts/report_sample.py > out.perf
```shell
$ simpleperf report
Cmdline: /system/bin/simpleperf record -g sleep 1
Arch: arm64
Event: cpu-cycles (type 0, config 0)
Samples: 56
Event count: 13885436
Overhead  Command     Pid    Tid    Shared Object                    Symbol
9.61%         sleep       14852  14852  [kernel.kallsyms]            vma_link
8.97%         sleep       14852  14852  linker64                     soinfo_do_lookup_impl
6.42%         sleep       14852  14852  linker64                     BionicAllocator::alloc_impl
6.11%         sleep       14852  14852  [kernel.kallsyms]            __follow_mount_rcu
5.83%         sleep       14852  14852  [kernel.kallsyms]            clear_page
...
```

### FlameGraph
将数据报告绘制程火焰图
> FlameGraph/stackcollapse-perf.pl out.perf > out.folded;FlameGraph/flamegraph.pl out.folded > graph.svg

![在这里插入图片描述](https://img-blog.csdnimg.cn/6c24103a5f214aa1ac9c88ad857e258d.gif#pic_center)

## XTS
* 选择本地xts套件
* 选择要执行的命令
* 选择执行模块（目前只支持camera模块）
* 输入要执行的test（输入为空，则运行整个模块）
* run cts
	> cts 运行结束之后会自动解析results/latest/result.xml，将结果显示在tableView中，并且会检索fail项的历史解决方案

![在这里插入图片描述](https://img-blog.csdnimg.cn/c33ae0c55ad8482c9e5f4d607812c445.gif#pic_center)
* 加载results文件
> 如果某一个fail项有历史解决方案，可以点击Resulotion栏中的连接直接转跳至相应网页

![在这里插入图片描述](https://img-blog.csdnimg.cn/c23251a5190d4f26b2e4a79a1b40dda2.gif#pic_center)


