﻿#ifndef CMD_H
#define CMD_H

//test
#define TEST1                   "ping baidu.com"
#define TEST2                   "pwd"

//COMMON
#define DATABASE                "config/native_cmd_list.txt"
#define HINT                    "\t\t Hint : press 'Ctrl + D' to stop!"
#define INITCONFIG              "config/oneclick.ini"    //不能使用:/config/config/oneclick.ini,会导致数据不更新,具体原因不得而知

//ADB
#define ADBDEVICES              "adb devices"
#define ADBROOT                 "adb root"
#define ADBREMOUNT              "adb remount"
#define ADBDISABLEVERITY        "adb disable-verity"
#define ADBOEMUNLOCK            "adb reboot bootloader;fastboot flashing unlock;fastboot reboot ;adb wait-for-device;adb root;adb disable-verity;adb remount;adb reboot ;adb wait-for-device;adb root;adb remount"
#define ADBRESTART              "adb kill-server;adb start-server"
#define FASTBOOT                "adb reboot bootloader"
#define CLEARLOGCAT             "adb logcat -c"

#define SENTEST                 "adb shell sentest"
#define KILLCAMERASERVER        "adb shell pkill camera*"
#define DUMPCAMERA              "adb shell dumpsys media.camera"
#define PROPERTY                "adb shell getprop"
#define DRAWID                  "adb shell setprop debug.cam.drawid 1"

#define SCREENCAPTURE           "adb shell screencap -p "
#define SCREENRECORD            "adb shell screenrecord "
#define OPENCAMERA              "adb shell am start -n com.tcl.camera/com.android.camera.CameraActivity"
#define TAKEPICTURE             "adb shell input tap 360 1400"
#define CPUINFO                 "adb shell cat proc/cpuinfo"
#define SCREENSIZE              "adb shell wm size"

#define BATTERYSTATS            "adb shell dumpsys batterystats "
#define BUGREPORT               "adb bugreport "
#define BATTERYHISTORIANURL     "<a href=\"https://bathist.ef.lc/\">BATTERY HISTORIAN"

//simpleperf
#define SIMPLEPERFDATAPATH      "simpleperf_data"
#define PERFIX                  "adb shell system/bin/simpleperf "
#define LIST                    "list"
#define STAT                    "stat -e cache-references,cache-misses -a --duration 10 "
#define RECORD                  "record -o data/local/tmp/perf.data -e task-clock:u -f 1000 -g --duration 10 --log info --app com.tcl.camera"
#define REPORT                  "python scripts/report_sample.py simpleperf_data/perf.data > simpleperf_data/out.perf"
#define FLAMEGRAPH              QString("FlameGraph/stackcollapse-perf.pl %1/out.perf > %1/out.folded;FlameGraph/flamegraph.pl %1/out.folded > %1/graph.svg").arg(SIMPLEPERFDATAPATH)
#define SIMPLEPERFDOC           "<a href=\"https://android.googlesource.com/platform/system/extras/+/master/simpleperf/doc/README.md\">Simpleperf Doc"

//xts
#define CTSTESTLIST             ":/config/config/cts_test_list.txt"
#define CTSRESULOTION           ":/config/config/cts_resulotion.csv"
#define TESTRESULT              ":/config/config/test_result.xml"
//#define CTSSUITE                "~/XTS/Android_R/android-cts/tools/cts-tradefed "
#define CTSCOMMAND              "run cts-dev "
#define CTSMODULE               "-m CtsCameraTestCases "
#define FINDCTSSUITE            QString("timeout 5 find %1 -name cts-tradefed")

//ssh
#define LIBLIST                 ":/config/config/lib_list.txt"
#define PROJECTFILE             ":/config/config/project.xml"
#define REPOSYNC                "repo sync -c --no-tag"
#define NINJA                   "./prebuilts/build-tools/linux-x86/bin/ninja -f "
#define SSHROOTDIR              "directory.txt"








#endif // CMD_H
