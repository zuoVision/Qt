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
    if(info.contains("exit")){
        qDebug()<<"FORCE EXIT";
        emit signalToSimpleperf("<font color=\"#ee0000\">FORCE EXIT!</font>");
        p->kill(); //强关
//        p->terminate();
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

void ListenerThread::exitCmd(int exitCode)
{
    qDebug() << MY_TAG  <<"[exitCmd]"
             << QThread::currentThreadId()
             << exitCode;

    return;
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
//    emit signalToMainWindow(m_hostName);


    qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
    connect(p,SIGNAL(stateChanged(QProcess::ProcessState)),
            this,SLOT(slotReciveProcessState(QProcess::ProcessState)));
}

void ListenerThread::process()
{
    p->start(m_program,m_cmd);
    while(p->state()!=QProcess::NotRunning){
        p->waitForFinished(1);
        qDebug()<<"atEnd:"<<p->atEnd();
        m_stdOutput = p->readAllStandardOutput();
        m_stdError = p->readAllStandardError();
        emit signalToSimpleperf(m_stdOutput);
        emit signalToSimpleperf(m_stdError);
        qDebug()<<"OUTPUT:"<<m_stdOutput;
        qDebug()<<"ERROR:"<<m_stdError;
        msleep(500);
    }
    qDebug()<<"state:"<<p->state();
    m_cmd.clear();
//    qDebug()<<"complete:"<<p->state();
//    m_cmd.clear();

//    if(p->state()==QProcess::Running){
//        sleep(1);
//        qDebug()<<MY_TAG<<"进程正在运行...";
//        emit signalToSimpleperf("进程正在运行...");
//        m_stdOutput = p->readAllStandardOutput();
//        emit signalToSimpleperf(m_stdOutput);
//    }else if(p->state()==QProcess::Starting){
//        qDebug()<<MY_TAG<<"进程正在启动...";
//        emit signalToSimpleperf("进程正在启动...");
//    }else if(p->state()==QProcess::NotRunning){
//        qDebug() << MY_TAG  <<"[run]"
//                 << m_cmd;

//        m_stdOutput = p->readAllStandardOutput();
//        m_stdError = p->readAllStandardError();
//        if(!m_stdOutput.isEmpty() ){
//            qDebug()<<"output:"<<m_stdOutput;
//            emit signalToSimpleperf(m_stdOutput);
//            m_stdOutput.clear();
//        }else if(!m_stdError.isEmpty()){
//            qDebug()<<"Error:"<<m_stdError;
//            emit signalToSimpleperf(m_stdError);
//            m_stdError.clear();
//        }
//        if(p->state()==QProcess::Running){
//            return;
//        }
//        emit signalToSimpleperf("END");
//        p->close();
//        m_cmd.clear();
//    }
}

void ListenerThread::slotReciveMainWindow(QStringList info)
{
    qDebug()<<MY_TAG<<"[slotReciveSimpleperf] #Thread ID:"
           <<QThread::currentThreadId() << info;
}


void ListenerThread::slotReciveSimpleperf(QStringList info)
{
    if(info.isEmpty()){
        emit signalToSimpleperf("cmd is empty");
        return;
    }
    if(isExit(info)){
        return ;
    }
    m_cmd=info;

    qDebug() << MY_TAG  <<"[slotReciveSimpleperf]"
             << QThread::currentThreadId() << info
             <<p->state();
}


