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