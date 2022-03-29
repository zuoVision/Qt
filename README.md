# Qt
Qt project

将本地代码提交至git 
- git push git@github.com:zuoVision/Qt.git master

将远程代码同步至本地
- git pull git@github.com:zuoVision/Qt.git

2022年3月8日 22点57分
- process 初始化 --> hostName 样式：zuozhe$

2022年3月9日
- 增加操作文档显示窗口

2022年3月10日
* 增加文本保存操作
* 完善process处理逻辑 程序强制退出，执行命令前判断当前是否有正在执行/未执行完成的命令

2022年3月11日
* 增加监听进程

2022年3月13日
* 实现进程间通信

2022年3月14日
* 实现子进程处理cmd

2022年3月15日
* 实现simpleperf类，待解决问题：process处理后无返回结果，为""

2022年3月16日
* 通过重载run(){while...}实现循环事件，实时监听process输出，类似于ping github.com的逐行显示到ui
* process处理无返回结果一般原因是：readAllStandardOutput只能读取一次，读取完之后就会清空输出，所以需要把readAllStandardOutput结果用QString保存起来

2022年3月19日
* 修复process不能实时输出的问题

2022年3月20日
* 增加命令行补全功能

2022年3月20日
* 构建cmd database，保存历史数据，供命令行补全

2022年3月21日
* 增加signalToSimpleperf的信号发送类型

2022年3月22日
* 优化cmd database存储方案
* 完善native cmd
* 新增补全方式选项等
* 新增process处理完成后发送信号的类型

2022年3月23日
* 新增dockwidget显示输出内容

2022年3月25日
* 创建XTS模块

2022年3月26日
* 创建commonCommand模块
* 重构process处理流程--> 改在commandprocessthread中执行 弃用原listenerthread
* 完善xts模块

2022年3月27日
* 完善xts模块，新增testcase result查看功能
* 状态栏分栏
* 调整debug log格式
* TODO:QThread: Destroyed while thread is still running

2022年3月28日
* 修改状态栏

2022年3月29日
* 修改xts逻辑
* TODO:QThread: Destroyed while thread is still running