#include "simpleperf.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>


#define MY_TAG "simpleperf"

Simpleperf::Simpleperf(QObject *parent):
QObject(parent)
{
    //实例化监听进程
//    listenerThread = new QThread();
    listener = new ListenerThread();
    init_connect();
    listener->start();
//    listener->moveToThread(listenerThread);
//    listenerThread->start();
//    emit signalToListenerInitThread();


}

void Simpleperf::init_connect()
{
//    connect(this,SIGNAL(signalToListenerInitThread()),
//            listener,SLOT(slotInitThread()));
    connect(this,SIGNAL(signalToListenerThread(QStringList)),
            listener,SLOT(slotReciveSimpleperf(QStringList)));
    connect(listener,SIGNAL(signalToSimpleperf(QString)),
            this,SLOT(slotReciveListener(QString)));
    connect(listener,SIGNAL(signalToSimpleperf(QProcess::ProcessState)),
            this,SLOT(slotReciveProcessState(QProcess::ProcessState)));
}

void Simpleperf::runCmdLine(QString cmd)
{
    m_cmd << "-c" << cmd ;
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();
}

void Simpleperf::runAdbDevices(QString cmd)
{

    m_cmd << "-c" << cmd;
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();
}

void Simpleperf::runAdbRoot(QString cmd)
{
    m_cmd << "-c" << cmd;
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();
}

void Simpleperf::runAdbRemount(QString cmd)
{
    m_cmd << "-c" << cmd;
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();
}

void Simpleperf::runAdbOemUnlock(QString cmd)
{
    m_cmd << "-c" << cmd;
//    emit signeixtalToListenerThread(m_cmd);
    m_cmd.clear();
}

void Simpleperf::runSimpleperfStat(QString cmd)
{
    m_cmd << "-c" << "ls";
    emit signalToListenerThread(m_cmd);

}

void Simpleperf::runSimpleperfRecord(QString cmd)
{
    qDebug() << MY_TAG << "runSimpleperfRecord";
    // 1. python
    //    QStringList m_cmd ;
    //    m_cmd << QCoreApplication::applicationDirPath() + "/scripts/app_profiler.py"
    //        << "-p" << "com.tcl.camera";
    //    excuteCmd(PYTHON2_7,m_cmd);

    //    2.bash
    m_cmd <<"-c" << cmd;
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();

    m_cmd << "-c" << "adb pull /data/local/tmp/perf.data";
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();

}

void Simpleperf::runSimpleperfReport(QString cmd)
{
    m_cmd <<"-c" << cmd;
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();
    emit signalToMainWindow("[runSimpleperfReport] finished");
    qDebug() <<MY_TAG << "[runSimpleperfReport] finished";
}

void Simpleperf::runflamegraph()
{
    m_cmd <<"-c" << "FlameGraph/stackcollapse-perf.pl out.perf > out.folded";
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();

    m_cmd <<"-c" << "FlameGraph/flamegraph.pl out.folded > graph.svg";
    emit signalToListenerThread(m_cmd);
    m_cmd.clear();
    if(QDesktopServices::openUrl(QUrl("graph.svg"))){
//        emit signalToMainWindow(/*fileName*/);
        emit signalToMainWindow("<font color=\"#00cc00\">FlameGraph Opened  </font>");
    }else {
        emit signalToMainWindow("<font color=\"#ee0000\">Open FlameGraph Failed !</font> ");
    }
}

void Simpleperf::slotReciveListener(QString msg)
{
//    qDebug() << MY_TAG <<"[slotReciveListener]" << msg;
    emit signalToMainWindow(msg);
}

void Simpleperf::slotReciveProcessState(QProcess::ProcessState newState)
{
    qDebug()<<MY_TAG<<"[slotReciveProcessState] ProcessState";
    emit signalToMainWindow(newState);
}



