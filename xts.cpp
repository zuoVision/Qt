#include "xts.h"

#include <QDataStream>
#include <QDebug>

#define MY_TAG          "XTS"
#define cout            qDebug() << MY_TAG << __BASE_FILE__ << __LINE__ << __FUNCTION__

#define CTSSUITE        "~/XTS/Android_R/android-cts/tools/cts-tradefed "
#define CTSCOMMAND      "run cts-dev "
#define CTSMODULE       "-m CtsCameraTestCases "
#define CTSTEST         "-t android.hardware.camera2.cts.RobustnessTest#testSimultaneousTriggers"
#define FINDCTSSUITE    "find ~/ -iname cts-tradefed"


Xts::Xts(QObject *parent) : QObject(parent)
{
    qDebug() << MY_TAG << "Xts()" <<QThread::currentThreadId();
    xts_Thread = new QThread(this);
    xts_cpt = new CommandProcessThread();
    init_connect();
    xts_cpt->moveToThread(xts_Thread);
    xts_Thread->start();
    emit start();
    QThread::sleep(1);
    findCtsSuite();
}

Xts::~Xts()
{
    qDebug()<<MY_TAG<<"~Xts() +";
    emit exit();
    delete xts_cpt;
    xts_Thread->exit(0);
    delete xts_Thread;
    qDebug()<<MY_TAG<<"~Xts() -";
}

void Xts::init_connect()
{
    connect(this,SIGNAL(start()),
            xts_cpt,SLOT(createProcessor()));
    connect(this,SIGNAL(processCommand(QString)),
            xts_cpt,SLOT(process(QString)));
    connect(this,SIGNAL(processCommand(QString,unsigned long)),
            xts_cpt,SLOT(process(QString,unsigned long)));
    connect(this,SIGNAL(stop()),
            xts_cpt,SLOT(stopProcessor()));
    connect(this,SIGNAL(exit()),
            xts_cpt,SLOT(exitProcessor()));
    connect(xts_cpt,SIGNAL(sig_sendOutput(QString)),
            this,SLOT(slo_reciveOutput(QString)));
    connect(xts_cpt,SIGNAL(sig_sendError(QString)),
            this,SLOT(slo_reciveError(QString)));
    connect(xts_cpt,SIGNAL(sig_sendInfo(QString)),
            this,SLOT(slo_reciveInfo(QString)));
    connect(xts_cpt,SIGNAL(sig_sendState(QProcess::ProcessState)),
            this,SLOT(slo_reciveState(QProcess::ProcessState)));

}

void Xts::findCtsSuite()
{
    qDebug() << MY_TAG << FINDCTSSUITE;
    emit processCommand(FINDCTSSUITE,3);
}

void Xts::slo_reciveOutput(QString output)
{
    qDebug() << MY_TAG << "slo_reciveOutput" << output;
    if (output.endsWith("tools/cts-tradefed")){
        m_ctsSuite << output.split("\n");
        qDebug() << m_ctsSuite;
        emit sig_findCtsSuite();
        return;
    }   
    if(output.contains("SuiteResultReporter: ")) analyzeResult(output);

    emit sig_sendToMainWindow(output);
}

void Xts::slo_reciveError(QString error)
{
//    qDebug() << MY_TAG << "slo_reciveError" << error;
    emit sig_sendToMainWindow(error);
}

void Xts::slo_reciveInfo(QString info)
{
//    qDebug() << MY_TAG << "slo_reciveInfo" << info;
    emit sig_sendToMainWindow(info);
}

void Xts::slo_reciveState(QProcess::ProcessState state)
{
//    qDebug() << MY_TAG << "slo_reciveState" << state;
    emit sig_sendToMainWindow(state);
    if (state==QProcess::ProcessState::NotRunning)
    {
        emit sig_sendToMainWindow("Done");
    }
}

void Xts::runCts(const QString arg1, const QString arg2, const QString arg3, const QString arg4)
{
        qDebug() << MY_TAG << "runCts"
                 << arg1 <<arg2 <<arg3 <<arg4;
    if (xts_cpt->processor->state() != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");

    QString ctssuite,ctscmd,ctsmodule,ctstest;

    !arg1.isEmpty() ? ctssuite = arg1                           : CTSSUITE;
    !arg2.isEmpty() ? ctscmd = QString(" ").append(arg2)        : CTSCOMMAND;
    !arg3.isEmpty() ? ctsmodule = QString(" -m ").append(arg3)  : CTSMODULE;
    !arg4.isEmpty() ? ctstest = QString(" -t ").append(arg4)    : " ";

    QString cmd = ctssuite+ctscmd+ctsmodule+ctstest;
    qDebug()<<MY_TAG<<cmd;
    emit sig_sendToMainWindow(xts_cpt->m_userName+cmd);
    emit processCommand(cmd);
}

void Xts::stopProcessor()
{
    emit stop();
}

void Xts::analyzeResult(QString output)
{
    QStringList list = output.split("\n");
    m_totalRunTime  = list.filter("Total Run time:").first();
    m_totalTests    = list.filter("Total Tests:").first();
    m_passed        = list.filter("PASSED:").first();
    m_failed        = list.filter("FAILED:").first();
    cout << m_totalRunTime << m_totalTests << m_passed << m_failed;

}


