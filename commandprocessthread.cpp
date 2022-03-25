#include "commandprocessthread.h"

#include <QDebug>


#define MY_TAG "CommandProcessThread"

CommandProcessThread::CommandProcessThread()
{
    qDebug() << MY_TAG << "CommandProcessThread" <<currentThreadId();


}

void CommandProcessThread::run()
{
    qDebug() << MY_TAG  <<"[run]"
             << QThread::currentThreadId();

    processors.push_back(p1);
    processors.push_back(p2);
    qDebug()<<MY_TAG<<"p1:"<<&p1;
    qDebug()<<MY_TAG<<"p2:"<<&p2;
    createProcessor();
    while (true) {
        qDebug() << MY_TAG << "run" << currentThreadId();
        process();
        sleep(1);
    }
}

void CommandProcessThread::process()
{
    qDebug() << MY_TAG << "process" << currentThreadId()<<"begin";
    processors[0]->start("bash",QStringList()<<"-c"<<"ls");
    processors[0]->waitForFinished();
    processors[1]->start("bash",QStringList()<<"-c"<<"pwd");
    processors[1]->waitForFinished();
    qDebug() << MY_TAG << "process" << currentThreadId()<<"end";

}

void CommandProcessThread::printOutput()
{
    qDebug()<<MY_TAG<<"printOutput"<< processors[0]->readAllStandardOutput();
    qDebug()<<MY_TAG<<"printOutput"<< processors[1]->readAllStandardOutput();

}

void CommandProcessThread::createProcessor()
{
    qDebug() << MY_TAG  <<"[createProcessor]"
             << QThread::currentThreadId();

//    QVector<QProcess*>::iterator it;
//    for(it=processors.begin();it!=processors.end();it++)
//    {
//        qDebug()<< MY_TAG << &it;
//        &it = new QProcess();
//        connect(*it,SIGNAL(readyReadStandardOutput()),
//                this,SLOT(printOutput()));
//    }

    for(int i=0;i<processors.size();i++){
        qDebug()<<MY_TAG<<"i:"<<i<<&processors[i];
        processors[i] = new QProcess();
        connect(processors[i],SIGNAL(readyReadStandardOutput()),
                this,SLOT(printOutput()));
    }

}

