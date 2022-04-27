#include "basemodule.h"
#include <QDebug>
#include <QThread>

#define MY_TAG "BaseModule"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

BaseModule::BaseModule(QObject *parent) : QObject(parent)
{
    cout <<QThread::currentThreadId();
    init();
}

BaseModule::~BaseModule()
{
    cout;
}

void BaseModule::init()
{
    cout;
//    mThread = new QThread(this);
//    mCpt = new CommandProcessThread();
//    onCreateThread(mThread,mCpt);
}

void BaseModule::uninit()
{
    cout;
}

void BaseModule::init_connect(CommandProcessThread * ptrCPT)
{
//    connect(this,SIGNAL(start()),
//            ptrCPT,SLOT(createProcessor()));
//    connect(this,SIGNAL(process(QString)),
//            ptrCPT,SLOT(process(QString)));
//    connect(ptrCPT,SIGNAL(sig_sendOutput(QString)),
//            this,SLOT(slo_reciveOutput(QString)));
//    connect(this,SIGNAL(stop()),
//            ptrCPT,SLOT(stopProcessor()));
//    connect(ptrCPT,SIGNAL(sig_sendError(QString)),
//            this,SLOT(slo_reciveError(QString)));
//    connect(ptrCPT,SIGNAL(sig_sendInfo(QString)),
//            this,SLOT(slo_reciveInfo(QString)));
//    connect(ptrCPT,SIGNAL(sig_sendState(QProcess::ProcessState)),
//            this,SLOT(slo_reciveState(QProcess::ProcessState)));
}

void BaseModule::onCreateThread(QThread * prtThead,CommandProcessThread * ptrCPT)
{
//    cout;
//    ptrCPT->moveToThread(prtThead);
//    prtThead->start();
//    emit start();
//    cout<<"create thread done.";
}

void BaseModule::onDestroyThread(QThread * prtThead)
{

}

void BaseModule::excute(CommandProcessThread * ptrCPT,const QString cmd)
{

}

void BaseModule::stopProcessor()
{

}

void BaseModule::slo_reciveOutput(QString output)
{
    cout << output;
    emit sig_sendToMainWindow(output);
}

void BaseModule::slo_reciveError(QString error)
{

}

void BaseModule::slo_reciveInfo(QString info)
{

}

void BaseModule::slo_reciveState(QProcess::ProcessState state)
{

}

void BaseModule::slo_reciveMainWindow(QString msg)
{

}

void BaseModule::slo_reciveProcessThread(QString msg)
{

}
