#include "xts.h"

#include <QDataStream>
#include <QDebug>

#define MY_TAG          "Xts"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]"

#define CTSSUITE        "~/XTS/Android_R/android-cts/tools/cts-tradefed "
#define CTSCOMMAND      "run cts-dev "
#define CTSMODULE       "-m CtsCameraTestCases "

#define FINDCTSSUITE    "find ~/ -iname cts-tradefed"


Xts::Xts(QObject *parent) : QObject(parent)
{
    cout << QThread::currentThreadId();
    xts_Thread = new QThread(this);
    xts_cpt = new CommandProcessThread();
    init_connect();
    xts_cpt->moveToThread(xts_Thread);
    xts_Thread->start();
    emit start();
    QThread::sleep(1);
}

Xts::~Xts()
{
    cout <<"~Xts() +";
    emit exit();
    delete xts_cpt;
    xts_Thread->exit(0);
    delete xts_Thread;
    cout<<"~Xts() -";
}

void Xts::init_connect()
{
    connect(this,SIGNAL(start()),
            xts_cpt,SLOT(createProcessor()));
    connect(this,SIGNAL(processCommand(QString)),
            xts_cpt,SLOT(process(QString)));
    connect(this,SIGNAL(processCommand(QString,QString *)),
            xts_cpt,SLOT(process(QString,QString *)));
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

void Xts::slo_reciveOutput(QString output)
{
    cout << output;
    if(output.contains("SuiteResultReporter: "))
        m_ctsComplete = true;
    if (m_flag && output.contains("cts-tradefed")) {
        QStringList res = output.split("\n").filter("cts-tradefed");
        if(res.isEmpty()){
            emit sig_sendToMainWindow("Warning : No suite detected, please reselect cts suite!");
            m_flag = false;
            return;
        }
        if(res.size()>1){
            emit sig_sendToMainWindow("Warning : Multiple suite detected, please reselect cts suite!");
            m_flag = false;
            m_ctsSuite.clear();
            return;
        }
        m_ctsSuite = res.first();
        return;
    }
    emit sig_sendToMainWindow(output);
}

void Xts::slo_reciveError(QString error)
{
//    cout << error;
    emit sig_sendToMainWindow(error);
}

void Xts::slo_reciveInfo(QString info)
{
//    cout << info;
    emit sig_sendToMainWindow(info);
}

void Xts::slo_reciveState(QProcess::ProcessState state)
{
    cout << state;
    emit sig_sendToMainWindow(state,MY_TAG);
    if (state==QProcess::ProcessState::NotRunning)
    {
        if(!m_flag)
            emit sig_sendToMainWindow("Done");
//        else{
//            if(m_ctsSuite.isEmpty()){
//                emit sig_sendToMainWindow("Warning : No suite detected, please reselect cts suite!");
//                m_flag = false;
//            }
//            if (m_ctsSuite.size()>1){
//                emit sig_sendToMainWindow("Warning : Multiple suite detected, please reselect cts suite!");
//                m_flag = false;
//                m_ctsSuite.clear();
//            }
//        }
        if(m_ctsComplete){
            emit sig_showCtsResult();
            m_ctsComplete=false;
        }
    }
}

void Xts::slo_reciveMainWindow(QString msg)
{
    m_ctsSuite.clear();
    processCommand(QString("find %1 -name cts-tradefed").arg(msg));
    m_flag = true;
}

void Xts::runCts(const QString arg1, const QString arg2, QString arg3, QString arg4)
{
    //arg1 deprecated
    cout << arg1 <<arg2 <<arg3 <<arg4;
    if (xts_cpt->processor->state() != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    if(m_ctsSuite.isEmpty()) {
        emit sig_sendToMainWindow("please select cts suite!");
        return;
    }
    if(!arg3.isEmpty()) {
        arg3 = QString(" -m %1").arg(arg3);
    }
    if(!arg4.isEmpty()){
        arg4 = QString(" -t %1").arg(arg4);
        cout << arg4;
    }
    QString cmd = m_ctsSuite +" "+arg2+arg3+arg4;
    cout << cmd;
    emit sig_sendToMainWindow(xts_cpt->m_userName+cmd);
    emit processCommand(cmd);
}

void Xts::stopProcessor()
{
    emit stop();
}

#if 0
void Xts::analyzeResult(QString output)
{
    QStringList list = output.split("\n");
    m_totalRunTime  = list.filter("Total Run time:").first();
    m_totalTests    = list.filter("Total Tests:").first();
    m_passed        = list.filter("PASSED:").first();
    m_failed        = list.filter("FAILED:").first();
    cout << m_totalRunTime << m_totalTests << m_passed << m_failed;
    emit sig_showCtsResult();
}
#endif
