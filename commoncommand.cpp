#include "commoncommand.h"

#include <QDebug>

#define MY_TAG "CommonCommand"


CommonCommand::CommonCommand(QObject *parent) : QObject(parent)
{
    qDebug() << MY_TAG << "[CommonCommand]" <<QThread::currentThreadId();
    init();
}

CommonCommand::~CommonCommand()
{
    qDebug() << MY_TAG << "~Simpleperf";
    delete ccd_cpt;
    delete ccd_Thread;
}

void CommonCommand::init()
{
    qDebug() << MY_TAG << "[CommonCommand]" <<QThread::currentThreadId();

    ccd_Thread = new QThread(this);
    ccd_cpt = new CommandProcessThread();
    init_connect();
    ccd_cpt->moveToThread(ccd_Thread);
    ccd_Thread->start();
    emit start();
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

void CommonCommand::slo_reciveOutput(QString output)
{
//    qDebug() << MY_TAG << "slo_reciveOutput" << output;
    emit sig_sendToMainWindow(output);
}

void CommonCommand::slo_reciveError(QString error)
{
//    qDebug() << MY_TAG << "slo_reciveError" << error;
    emit sig_sendToMainWindow(error);
}

void CommonCommand::slo_reciveInfo(QString info)
{
//    qDebug() << MY_TAG << "slo_reciveInfo" << info;
    emit sig_sendToMainWindow(info);
}

void CommonCommand::slo_reciveState(QProcess::ProcessState state)
{
//    qDebug() << MY_TAG << "slo_reciveState" << state;
    emit sig_sendToMainWindow(state);
    if (state==QProcess::ProcessState::NotRunning)
        emit sig_sendToMainWindow("Done");
}

void CommonCommand::runCommand(const QString cmd)
{
//    qDebug() << MY_TAG << "runCommand";
    if(!cmd.isEmpty()&&
        ccd_cpt->processor->state()==QProcess::ProcessState::NotRunning)
        emit sig_sendToMainWindow(ccd_cpt->m_userName+cmd);
        emit processCommand(cmd);
}

void CommonCommand::stopProcessor()
{
    emit stop();
}
