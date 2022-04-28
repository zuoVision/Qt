#include "commoncommand.h"

#include <QDebug>

#define MY_TAG "CommonCommand"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

CommonCommand::CommonCommand(QObject *parent) : QObject(parent)
{
    cout <<QThread::currentThreadId();
    init();
}

CommonCommand::~CommonCommand()
{
    cout ;
    delete ccd_cpt;
    delete ccd_Thread;
}

void CommonCommand::init()
{
    cout <<QThread::currentThreadId();
    ccd_Thread = new QThread(this);
    ccd_cpt = new CommandProcessThread("commonCommand");
    init_connect();
    onCreateThread(ccd_Thread,ccd_cpt);
}

void CommonCommand::init_connect()
{
    connect(this,SIGNAL(start()),
            ccd_cpt,SLOT(createProcessor()));
    connect(this,SIGNAL(processCommand(QString)),
            ccd_cpt,SLOT(process(QString)));
    connect(ccd_cpt,SIGNAL(sig_sendOutput(QString)),
            this,SLOT(slo_reciveOutput(QString)));
    connect(this,SIGNAL(stop()),
            ccd_cpt,SLOT(stopProcessor()));
    connect(ccd_cpt,SIGNAL(sig_sendError(QString)),
            this,SLOT(slo_reciveError(QString)));
    connect(ccd_cpt,SIGNAL(sig_sendInfo(QString)),
            this,SLOT(slo_reciveInfo(QString)));
    connect(ccd_cpt,SIGNAL(sig_sendState(QProcess::ProcessState)),
            this,SLOT(slo_reciveState(QProcess::ProcessState)));
}

void CommonCommand::onCreateThread(QThread * prtThead,CommandProcessThread * ptrCPT)
{
    ptrCPT->moveToThread(prtThead);
    prtThead->start();
    emit start();
}

void CommonCommand::slo_reciveOutput(QString output)
{
    cout << output;
    emit sig_sendToMainWindow(output);
}

void CommonCommand::slo_reciveError(QString error)
{
    cout << error;
    emit sig_sendToMainWindow(error);
}

void CommonCommand::slo_reciveInfo(QString info)
{
    cout << info;
    emit sig_sendToMainWindow(info);
}

void CommonCommand::slo_reciveState(QProcess::ProcessState state)
{
    cout << state;
    emit sig_sendToMainWindow(state,MY_TAG);
    if (state==QProcess::ProcessState::NotRunning)
        emit sig_sendToMainWindow("Done");
}

void CommonCommand::runCommand(const QString cmd)
{
    cout;
    if(!cmd.isEmpty()&&
        ccd_cpt->processor->state()==QProcess::ProcessState::NotRunning){
        emit sig_sendToMainWindow(ccd_cpt->m_userName+cmd);
        emit processCommand(cmd);
    }else{
        cout << "command is empty or process is running!";
    }
}

void CommonCommand::stopProcessor()
{
    emit stop();
}
