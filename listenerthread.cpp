#include "listenerthread.h"
#include <QDebug>

#define MY_TAG "listenerthread"
#define TERMINAL "/usr/bin/terminator"
#define GNOME "/etc/gnome"

ListenerThread::ListenerThread(/*QObject *parent*/)
//QObject(parent)
{
    qDebug() << MY_TAG  <<"[ListenerThread]"
             << QThread::currentThreadId();

}

bool ListenerThread::isExit(QStringList info)
{
    if(/*p->state() != QProcess::NotRunning && */(info.contains("exit") || info.contains("CTRL + D")))
    {
        p->kill(); //强关
        qDebug()<<"FORCE EXIT";
        emit signalToSimpleperf("<font color=\"#ee0000\">FORCE EXIT!</font>");
        return true;
    }
    return false;
}

void ListenerThread::run()
{
    qDebug() << MY_TAG  <<"[run]"
             << QThread::currentThreadId();
    p = new QProcess();
    initThread(p);
    while (true) {
        if(!m_cmd.isEmpty()){
            process();
        }
    }
}

void ListenerThread::openTerminal()
{
//    QProcess *bash = new  QProcess();
//    bash->start(TERMINAL);
//    bash->waitForStarted();
//    bash->write("~/test.sh");    //向终端写入命令，注意尾部的“\n”不可省略
//    bash->write("\n\r");
    system("terminator");
}


void ListenerThread::slotReciveProcessState(QProcess::ProcessState newState)
{
    emit signalToSimpleperf(newState);
}


void ListenerThread::initThread(QProcess *processer)
{   
    qDebug() << MY_TAG  <<"[InitThread]"
             << QThread::currentThreadId();
    processer->start(BASH,QStringList() << "-c" << "whoami");    //hostname:41000966-26-1 username:zuozhe
    processer->waitForFinished();
    m_userName = processer->readAllStandardOutput().simplified()/*.append("$ ")*/;
    processer->start(BASH,QStringList()<< "-c" << "hostname");
    processer->waitForFinished();
    m_hostName = processer->readAllStandardOutput().simplified()/*.append("$ ")*/;
    qDebug() <<MY_TAG <<m_userName<<m_hostName;
    qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
    connect(processer,SIGNAL(stateChanged(QProcess::ProcessState)),
            this,SLOT(slotReciveProcessState(QProcess::ProcessState)));
}

void ListenerThread::process()
{
    p->start(m_program,m_cmd);
    while(p->state()!=QProcess::NotRunning){
        p->waitForFinished(1);
        m_stdOutput = p->readAllStandardOutput();
        m_stdError = p->readAllStandardError();
        if (!m_stdOutput.isEmpty()) emit signalToSimpleperf(m_stdOutput,OUTPUT_INFO);
        if (!m_stdError.isEmpty()) emit signalToSimpleperf(m_stdError,ERROR_INFO);
//        qDebug()<<"OUTPUT:"<<m_stdOutput;
//        qDebug()<<"ERROR:"<<m_stdError;
        msleep(500);
    }
    qDebug()<<"state:"<<p->state();
    emit signalProcessFinished();
    m_cmd.clear();
}

void ListenerThread::creatProcesser(QProcess *processer)
{
    processer->start(m_program,QStringList()<<"-c"<<"adb shell getprop ro.product.device");
    processer->waitForFinished();
    QString sig = processer->readAllStandardOutput();
    qDebug()<<sig;
    emit signalToSimpleperf(sig.append(":/ $ "));
}

void ListenerThread::slotReciveMainWindow(QStringList info)
{
    qDebug()<<MY_TAG<<"[slotReciveSimpleperf] #Thread ID:"
           <<QThread::currentThreadId() << info;
}


void ListenerThread::slotReciveSimpleperf(QStringList info)
{
    qDebug() << MY_TAG  <<"[slotReciveSimpleperf]"
             << QThread::currentThreadId() << info
             << p->state();
    if(info.isEmpty()||isExit(info)){

        return;
    }
//    if(!m_isSubConsoleOn && info.contains("adb shell")){
//        QProcess *as = new QProcess();
//        creatProcesser(as);
//        m_isSubConsoleOn=true;
//        return;
//    }
//    if(m_isSubConsoleOn){
//        m_cmd << info.first() << QString("adb shell ").append(info.last());
//        emit signalToSimpleperf(info.last());
//        qDebug()<< m_cmd;
//        return;
//    }
    m_cmd=info;


}


