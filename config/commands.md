# 命令大全

## adb

在Android源码下有adb：/Seattle_R/out/soong/host/linux-x86/bin/adb

adb 重启

`adb kill-server`  &&　`adb start-server`

关机

adb shell reboot -p

`adb version` 查看adb版本



```adb push <pc端文档路径> <手机端路径>```
​		```adb pull```

安装软件
​		```adb install [-l][-r][-s] <apkname.apk>```

`-l:means forward-lock the app #锁定该程序`

`-r:means reinstall the app ,keeping its data #重新安装该程序，保留数据`

`-s:means install on SD card instead of internal storage #安装在SD卡内，而不是设备内部存储`

卸载软件

`adb uninstall [-k] <package>`

`-k:means keep the data and cache directories #不删除程序运行所产生的数据和缓存目录`



启动root权限
​		```adb root ```

```adb remount```

查看连接设备
​		``adb devices``

查看进程
​		``adb shell ps | grep cam``

杀死进程
​		`adb shell kill <进程号>`

`adb dumpsys media.camera`

以fastboot模式重启
​		`adb reboot bootloader`

fastboot模式下查看连接设备
​		`fastboot devices`

写入镜像
​		`fastboot flash dtbo dtbo.img `

fastboot模式下的重启
​		`fastboot reboot`

查看sensor是否probe成功

`adb shell dmesg -w | grep -ie Probe`

设备的日志

`adb logcat` 

查看kernel层log

`adb logcat -b kernel`

过滤多个关键词

`adb logcat | grep  -ie camx -ie chiusecase`

或`adb logcat | egrep -i "CHIUSECASE\|CamX"`

`adb logcat | grep -E "CHIUSECASE|CamX"`

查看当前sensor name\ sensor id

`adb logcat | grep -ie SensorName`

查看log缓存大小

`adb logcat -g`

查看bug报告

adb bugreport 

adb disable-verity 

adb enable-verity

adb shell pm list packages   //列出手机上所有packages

adb shell dumpsys  <package>

adb shell am start -n <package_name>/.<activity_class_name>　　// 启动应用

>   adb shell am start -n com.jrdcom.mtf/.MainActivity	//命令打开MTF
>
>   adb shell am start -n com.tcl.camera/com.android.camera.CameraActivity    //打开camera

adb shell top 　　　　　  // 查看设备cpu和内存占用情况

adb shell top -m 6 　　  // 查看占用内存前6的app

adb shell top -n 1 　　　// 刷新一次内存信息，然后返回

adb shell procrank　　   // 查询各进程内存使用情况

adb shell kill [pid] 　　　// 杀死一个进程

adb shell ps 　　　　　  // 查看进程列表

adb shell ps -x [PID]    // 查看指定进程状态

adb shell service list 　 // 查看后台services信息

adb shell wm size  		 //查看屏幕尺寸

adb shell am monitor   // 获取应用包名 （先输命令，在打开想要查看的应用）

adb shell setenforce 0

>  setenforce 0 设置SELinux 成为permissive模式 临时关闭selinux的
>
> 在eng/userdebug版本中
> 使用setenforce 命令进行设置:
> adb shell setenforce 0 //设置成permissive 模式
> adb shell setenforce 1 //设置成enforce 模式
> 注意此方法重启后失效

截屏

`adb shell screencap -p /sdcard/screensnap1.png`

录屏

`adb shell screenrecord storage/sdcard0/screen.mp4`

 查看平台信息

`adb shell cat /proc/cpuinfo`

查看手机安装的APK

`adb shell pm list package`

查看APK的安装路径

`adb shell pm path apk.name`

如：adb shell  pm path TctCamera.apk

点击屏幕

adb shell input tap 360 1400  //拍照

adb shell input tap 600 1285  //auto -> protrait

adb shell input tap 100 1285  //protrait -> auto



## adb 报错

adb: error while loading shared libraries: libcutils.so.0: cannot open shared object file: No such file or directory

查看缺少那些文件

```ldd $(which adb)```

![image-20220107141451159](../image/Typora/typora-user-images/image-20220107141451159.png)

找出源文件

find / -type f -name "libcutils.so.0"

创建链接

ln -s /usr/lib/android/libcutils.so.0 /usr/lib/x86_64-linux-gnu/android/libcutils.so.0



## 查看sensor型號

adb shell sentest

# Logcat 打印等级(高通)

##　 camx

修改logcat打印等级

opengrok搜索：CamxLogGroupCore

在camxtypes.h文件可以看到这些log的定义

![image-20201112211038069](../image/Typora/typora-user-images/image-20201112211038069.png)

```xml
/vendor/qcom/proprietary/camx/src/settings/lito/camxsettings.xml
```

![image-20201113094218335](../image/Typora/typora-user-images/image-20201113094218335.png)

例如打印HAL 的INFO级别

![image-20201112211300173](../image/Typora/typora-user-images/image-20201112211300173.png)

找到CamxLogGroupHAL = (1 << 7);  # 表示1左移7位：10000000   = 2 ^ 7  , 转换16进制：0x80

方法一：修改手机中的文件：vendor/etc/camera/camxoverridesettings.txt中的logInfoMask=0x80 （大于这个值也行），然后push回去，**杀camera provider**，重新打印logcat即可！

>   camxoverridesettings文件的读取是在camx模块初始化时(服务启动时),所
>   以通过写文件的方式更改log等级需要重启Camera服务,并且要开启Camera后才能生效。
>
>   也可以采用这种方式修改：adb shell "echo logVerboseMask=0x2 >> vendor/etc/camera/camxoverridesettings.txt"

方法二：adb shell setprop persist.vendor.camera.logInfoMask 0x2000000，想要恢复的话再把logInfoMask值改回来，通过adb shell getprop persist.vendor.camera.logInfoMask可以查看是否设置成功！



>   
>
>   | Description         | Name             | Default Value | Setprop                                |
>   | ------------------- | ---------------- | ------------- | -------------------------------------- |
>   | Verbose Log Mask    | logVerboseMask   | 0             | persist.vendor.camera.logVerboseMask   |
>   | Entry/Exit Log Mask | logEntryExitMask | 0             | persist.vendor.camera.logEntryExitMask |
>   | Info Log Mask       | logInfoMask      | 0xFFFFFFFF    | persist.vendor.camera.logInfoMask      |
>   | Warning Log Mask    | logWarningMask   | 0xFFFFFFFF    | persist.vendor.camera.logWarningMask   |
>   | System Log Enable   | systemLogEnable  | TRUE          | persist.vendor.camera.systemLogEnable  |
>
>   

![image-20201117171823282](../image/Typora/typora-user-images/image-20201117171823282.png)

计算器16进制

<img src="../image/Typora/typora-user-images/image-20201112203811530.png" alt="image-20201112203811530" style="zoom:25%;" />



## chi-cdk

>   源码路径：/vendor/qcom/proprietary/chi-cdk/api/utils/chxdebugprint.h

各种等级log mask

![image-20210310170656718](../image/Typora/typora-user-images/image-20210310170656718.png)

修改camxoverridesettings.txt中`overrideLogLevels`

注：如果要同时打印ERROR && WARN log，那么overrideLogLevels=3    #1+2

---

## shell

查看文档，过滤关键字，并用颜色显示
​		`cat logcat3.log | grep -ie camx -ie " 669 " --color`

查看文件夹大小

`du -sh <文件夹>`

查找

`Ctrl+shift+F`

过滤

`grep -Rn '关键词' .` #在当前目录下的所有文件中搜索关键词  -R:递归 -n:显示行号

`grep -e` #只能传递一个参数，在单条命令中使用多个-e 可以实现OR操作

`grep -v`  #过滤掉不想看到的东西

`grep -i` #忽略大小写

`| grep keyword1 | grep keyword2` #与操作

`| grep  -ie camx -ie chiusecase`  #或操作

`| egrep -i "CHIUSECASE|CamX"`        #或操作

`grep -A 5 keyword` #显示关键字行及后5行

`grep -B 5 keyword` #显示关键字行及前5行

`grep -C 5 keyword` #显示关键字行及前后5行

---

## git

git安装
​		`sudo apt-get install git-core`

git配置
​		`git config --global user.name zhe.zuo #配置用户名`
​		`git config --global user.email zhe.zuo@tcl.com #配置用户邮箱`

查看git配置
​		`git config -l `

.git配置文件
​		HEAD   		--指向当前分支
​		objects  	--储存所有commit ID
​		refs        	-- 储存指向分支的提交对象的指针

创建一个版本库
​		`git init db`

添加文件到版本库
​		`git add <file>`

查看git状态
​		`git status`

提交内容到版本库
​		`git commot -m “Inital commit of gittutor repository`

比较工作目录和版本库中的差异
​		`git diff <file>`

组合命令 -- 直接将工作提交到版本库
​		`git commit -a -m “new day for git”`

创建分支
​		`git branch visvim`

切换分支
​		`git checkout visvim`

退回到之前的某个版本

`git reset --hard 1dcd658020307b1a9c519976e7c3c75f7556a037`

查看本地分支
​		`git branch`

删除本地分支
​		`git branch -D visvim`

查看当前工作分支
​		`cat .git/HEAD`

查看每个分支的世系发展状态
​		``git show-branch``

查看分支的发展
​		`git whatchanged`

repo安装
​		`git clone git@10.92.32:tools/repo`

初始化
​		`repo init`

项目下载
​		`repo init -u git@ningbo.gitweb.com:quicl/manifests -m p6150.xml`

项目恢复初始状态

`repo forall -c "git reset --hard HEAD";repo forall -c "git clean -df";rm -rf out`

chi-cdk下载

​		`git clone git@172.16.11.162:/qualcomm/platform/vendor/qcom/proprietary/chi-cdk.git -b q6150_dev`

更新本地代码

$ git pull  <远程主机名> <远程分支名>:<本地分支名>

`git pull origin mtk6833-s-v1.0-mp:remotes/origin/mtk6833-s-v1.0-mp`

git clean -f  删除untrack文件

git clean -fd  删除untrack文件/文件夹



提交的代码被abandon掉，想撤回本地重新git push:

*   git log 查看commit ID ，找到你提交的上一个commit ID

*   git reset --soft <commit ID>![image-20210426092701013](../image/Typora/typora-user-images/image-20210426092701013.png)

    >   git reset --hard <commit id> ：这种会直接将你本地的代码退回到上一个版本，导致你本地修改全无。
    
*   git stash   #备份当前工作区的内容，保存到git栈中

*   git stash list  #查看所有stash内容

*   git stash pop  #从git栈中获取最近一次的stash，恢复到工作区的内容。

*   git blame filename #查看每行代碼是誰修改的

## repo

### android源码查看所有分支切换分支

cd .repo/manifests

git branch -a

repo init -b android-4.1.2_r1

repo sync

## 手动提交代码

**git add** 

git add .

**git commit**

根目录下创建并修改commitmsg文件 

```
[2 of 2][11653801]cruze&cruzelites Modify the flow of dual-camera data reading
     ###%%%comment:[2 of 2]cruze&cruzelites Modify the flow of dual-camera data reading
     ###%%%bug number:11653801
     ###%%%product name:mtk6833-s-v1.0-dint
     ###%%%root cause:Coding
     ###%%%root cause detail: coding
     ###%%%Bug category:TCT
     ###%%%Test_Suggestion:test test
     ###%%%Test_Report:pass pass
     ###%%%Module_Impact:NO
     ###%%%Solution:NO
     ###%%%Pre_build:NO
     ###%%%author email:zhe.zuo@tcl.com
```

git commit -F ~/commitmsg   

**git push** 

git push ssh://zhe.zuo@hz.gerrit.tclcom.com:29418/mtk/alps/vendor/mediatek/proprietary/hardware/mtkcam3 HEAD:refs/for/mtk6833-s-v1.0-dint

git push ssh://zhe.zuo@sz.gerrit.tclcom.com:29418/qualcomm/vendor/tct/tctcameraalgo HEAD:refs/for/s_dev  //算法上移



---

# Typora

删除线：~~ddd~~

下划线：ctrl + u

表格：ctrl + t

插入图片：ctrl + shift + i

超链接：ctrl + k



---

# 快编

\### 单文件编译,快速编译 
./prebuilts/build-tools/linux-x86/bin/ninja -f out/combined-ottawa.ninja  <文件节点名> 
./prebuilts/build-tools/linux-x86/bin/ninja -f out/combined-ottawa.ninja  com.tct.node.scenedetect 
./prebuilts/build-tools/linux-x86/bin/ninja -f  out/combined-richland.ninja com.qti.sensor.richland_ov48b2q 
./prebuilts/build-tools/linux-x86/bin/ninja -f  out/combined-richland.ninja com.qti.sensormodule.richland_ov48b2q 
./prebuilts/build-tools/linux-x86/bin/ninja -f  out/combined-richland.ninja com.qti.chi.override 
./prebuilts/build-tools/linux-x86/bin/ninja -f  out/combined-richland.ninja camera.qcom 
或者用脚本(脚本在我服务器~*/bin/* ) 
mkninjia com.tct.node.scenedetect 

\### Pipeline快速编译 
python  vendor/qcom/proprietary/chi-cdk/tools/usecaseconverter/usecasecomposer.py  -i  vendor/qcom/proprietary/chi-cdk/oem/qcom/topology/mimas/bengal/bengal_usecase.xml  -o  vendor/qcom/proprietary/chi-cdk/oem/qcom/topology/mimas/bengal/g_bengal_usecase.xml  -t bengal -d  vendor/qcom/proprietary/chi-cdk/oem/qcom/topology/mimas/usecase-components/  -H vendor/qcom/proprietary/chi-cdk/tools/usecaseconverter/xmlheader.txt  && perl  vendor/qcom/proprietary/chi-cdk/tools/usecaseconverter/usecaseconverter.pl  vendor/qcom/proprietary/chi-cdk/oem/qcom/topology/mimas/common/common_usecase.xml  vendor/qcom/proprietary/chi-cdk/oem/qcom/topology/mimas/bengal/g_bengal_usecase.xml  vendor/qcom/proprietary/chi-cdk/core/lib/common/g_pipelines.h &&  ~/bin/mkninja.sh com.qti.chi.override -j 

这两个命令是快速编译pipeline xml的，配合张虹上次发的 
./prebuilts/build-tools/linux-x86/bin/ninja -f  out/combined-richland.ninja com.qti.chi.override 

\### 快速编译sensor bin 文件 
python vendor/qcom/properity/chi-cdk/tools/buildbins/buildbins.py  --yaml-file-name=buildbins_t1_le.yaml 
编译完成后在chi-cdk/oem/qcom/bin/ 中可以找到制定的sensor bin文件



---

# 9008

1.  连接数据线，长按 vol + && vol - && power 三个键，直到手机出现download模式，长按vol +
2.  如果还不行，3个按键同时按，震动后，放掉开机按键，4秒放掉vol - ，6秒后松开全部按键



# 宏定义

```
#if defined(Macro1) || defined(Macro2) || defined(Macro3)
...程序段1...
#elif defined(Macro4)
...程序段2...
#endif
```

## #ifdef && #if defined 的区别

区别在于：`#if defined`可以组成复杂的预编译条件，而`#ifdef`只能在两者中选择是否有定义

```
#if defined xxx1

#elif defined xxx2

#elif defined xxx3

#else

#endif
```

```
#ifdef xxx1

#else

#endif
```





---

重启smb

sudo /etc/init.d/smbd restart



---

反编译backtrace

将backtrace代码复制保存到项目根目录 tmp.log(新建文件)

$ development/scripts/stack tmp.log

![image-20210325200047200](../image/Typora/typora-user-images/image-20210325200047200.png)



## rename

#把文件名中的AA替换成aa

rename "s/AA/aa/" *

## sed

#将文件中的AA替换成aa

sed -i "s/AA/aa/g" *



## find

find  -name "*" | grep -i camname



## 暗码

![image-20210819155915570](../image/Typora/typora-user-images/image-20210819155915570.png)

## MTK 离线log

*# *#825364

adb root;adb shell am start -W -n com.mediatek.engineermode/.EngineerMode

选择Log and Debugging -> DebugLoggerUI



## disable-verity

打开OEMunlock

adb reboot bootloader;fastboot flashing unlock;fastboot reboot ;adb wait-for-device;rt;adb disable-verity;rmt;adb reboot ;adb wait-for-device;rtrmt



## 查看app下的size(MTK)

`adb logcat | grep -ie mtkcam-PipelineModelSession-Factory`

```
09-05 05:23:39.550  8220  8279 D mtkcam-PipelineModelSession-Factory: [parseAppStreamInfo] ispTuningDataEnable(0) StreamId(0x6) ConsumerUsage(2304) size(1280,720) yuv_isp_tuning_data_size(1280,720) raw_isp_tuning_data_size(2560,1920)
09-05 05:23:39.550  8220  8279 D mtkcam-PipelineModelSession-Factory: [parseAppStreamInfo] ispTuningDataEnable(0) StreamId(0x7) ConsumerUsage(65536) size(1920,1080) yuv_isp_tuning_data_size(1280,720) raw_isp_tuning_data_size(2560,1920)
```



## 如何 dump Jpeg ？以及 dump 后的保存路径在哪里？

*adb root 
adb remount
adb shell rm -rf /data/vendor/camera_dump/* 
adb shell setprop vendor.debug.camera.dump.JpegNode 1 
adb shell stop camerahalserver
adb shell start camerahalserver dump 完后，dump 出来的 buffer 都放到 /data/vendor/camera_dump 这个目录下了。 pull dump 出的 jpeg 到本地： adb pull /data/vendor/camera_dump <本地路径>



## tar

tar -xvf   xxxx.tar  解压



## 切换Python

`ln -s  /usr/bin/pythonx.x  /usr/bin/python`

## 安装package时不满足依赖关系

`sudo apt-get install -f` #修复依赖关系



## Python 环境切换

```
$ sudo update-alternatives --install /usr/bin/python python /usr/bin/python2.7 2  # 添加Python2可选项，优先级为2 
$ sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 1 #添加Python3可选项，优先级为1

#查看python优先级，并手动配置优先级
sudo update-alternatives --config python
```





## token

``*#*#43886536#*#*``