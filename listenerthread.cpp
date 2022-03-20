#include "listenerthread.h"
#include <QDebug>


#define MY_TAG "listenerthread"

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
    initThread();
    while (true) {
        if(!m_cmd.isEmpty()){
            process();
        }
    }

}


void ListenerThread::slotReciveProcessState(QProcess::ProcessState newState)
{
    emit signalToSimpleperf(newState);
}


void ListenerThread::initThread()
{   
    qDebug() << MY_TAG  <<"[InitThread]"
             << QThread::currentThreadId();
    p = new QProcess();
    p->start(BASH,QStringList() << "-c" << "whoami");
    p->waitForFinished();
    m_hostName = p->readAllStandardOutput().simplified().append("$ ");
    qDebug() <<MY_TAG <<m_hostName;
    qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
    connect(p,SIGNAL(stateChanged(QProcess::ProcessState)),
            this,SLOT(slotReciveProcessState(QProcess::ProcessState)));
}

void ListenerThread::process()
{
    p->start(m_program,m_cmd);
    while(p->state()!=QProcess::NotRunning){
        p->waitForFinished(1);
        m_stdOutput = p->readAllStandardOutput();
        m_stdError = p->readAllStandardError();
        emit signalToSimpleperf(m_stdOutput);
        emit signalToSimpleperf(m_stdError);
        qDebug()<<"OUTPUT:"<<m_stdOutput;
        qDebug()<<"ERROR:"<<m_stdError;
        msleep(500);
    }
    qDebug()<<"state:"<<p->state();
    emit signalProcessFinished();
    m_cmd.clear();
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
    if(info.isEmpty()){
        emit signalToSimpleperf("cmd is empty");
        return;
    }
    if(isExit(info)){
        return ;
    }
    m_cmd=info;


}


