#include "simpleperf.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>


#define MY_TAG "Simpleperf"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]"

#define SIMPLEPERFSTAT   "pwd"
#define SIMPLEPERFRECORD "python scripts/app_profiler.py -p com.tcl.camera"
#define SIMPLEPERFREPORT "python scripts/report_sample.py > out.perf"


Simpleperf::Simpleperf(QObject *parent):
QObject(parent)
{
    qDebug() << MY_TAG << "Simpleperf" <<QThread::currentThreadId();
    sim_Thread = new QThread(this);
    sim_cpt = new CommandProcessThread();
    init_connect();
    sim_cpt->moveToThread(sim_Thread);
    sim_Thread->start();
    emit start();

}

Simpleperf::~Simpleperf()
{
    qDebug() << MY_TAG << "~Simpleperf +";
    sim_Thread->exit(0);
    delete sim_cpt;
    delete sim_Thread;
    qDebug() << MY_TAG << "~Simpleperf -";
}
void Simpleperf::init_connect()
{
    connect(this,SIGNAL(start()),
            sim_cpt,SLOT(createProcessor()));
    connect(this,SIGNAL(processCommand(QString)),
            sim_cpt,SLOT(process(QString)));
    connect(this,SIGNAL(stop()),
            sim_cpt,SLOT(stopProcessor()));
    connect(sim_cpt,SIGNAL(sig_sendOutput(QString)),
            this,SLOT(slo_reciveOutput(QString)));
    connect(sim_cpt,SIGNAL(sig_sendError(QString)),
            this,SLOT(slo_reciveError(QString)));
    connect(sim_cpt,SIGNAL(sig_sendInfo(QString)),
            this,SLOT(slo_reciveInfo(QString)));
    connect(sim_cpt,SIGNAL(sig_sendState(QProcess::ProcessState)),
            this,SLOT(slo_reciveState(QProcess::ProcessState)));
}

void Simpleperf::slo_reciveOutput(QString output)
{
    qDebug() << MY_TAG << "slo_reciveOutput" << output;
    emit sig_sendToMainWindow(output);
}

void Simpleperf::slo_reciveError(QString error)
{
//    qDebug() << MY_TAG << "slo_reciveError" << error;
    emit sig_sendToMainWindow(error);
}

void Simpleperf::slo_reciveInfo(QString info)
{
//    qDebug() << MY_TAG << "slo_reciveInfo" << info;
    emit sig_sendToMainWindow(info);
}

void Simpleperf::slo_reciveState(QProcess::ProcessState state)
{
//    qDebug() << MY_TAG << "slo_reciveState" << state;
    emit sig_sendToMainWindow(state,MY_TAG);
    if (state==QProcess::ProcessState::NotRunning)
        emit sig_sendToMainWindow("Done");
    m_state = state;
}

void Simpleperf::runStat()
{
    qDebug() << MY_TAG << "runStat";
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    emit sig_sendToMainWindow(sim_cpt->m_userName+SIMPLEPERFSTAT);
    emit processCommand(SIMPLEPERFSTAT);
}

void Simpleperf::runRecord()
{
    qDebug() << MY_TAG << "runRecord";
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    emit sig_sendToMainWindow(sim_cpt->m_userName+SIMPLEPERFRECORD);
    // 1. python
    //    QStringList m_cmd ;
    //    m_cmd << QCoreApplication::applicationDirPath() + "/scripts/app_profiler.py"
    //        << "-p" << "com.tcl.camera";
    //    excuteCmd(PYTHON2_7,m_cmd);

    //    2.bash
    QString cmd1 = "adb shell rm /data/local/tmp/perf.data";
    QString cmd2 = SIMPLEPERFRECORD;
    QString cmd3 = "adb pull /data/local/tmp/perf.data";
    emit processCommand(cmd1+";"+cmd2+";"+cmd3);

}

void Simpleperf::runReport()
{
    qDebug() << MY_TAG << "runReport";
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    emit sig_sendToMainWindow(sim_cpt->m_userName+SIMPLEPERFREPORT);
    emit processCommand(SIMPLEPERFREPORT);
}


void Simpleperf::runFlamegraph()
{
    qDebug() << MY_TAG << "runFlamegraph";
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    QString cmd1 = "FlameGraph/stackcollapse-perf.pl out.perf > out.folded";
    QString cmd2 = "FlameGraph/flamegraph.pl out.folded > graph.svg";
    emit sig_sendToMainWindow(sim_cpt->m_userName+cmd1+cmd2);
    emit processCommand(cmd1+";"+cmd2);

    if(QDesktopServices::openUrl(QUrl("graph.svg"))){
        emit sig_sendToMainWindow("FlameGraph Opened");
//        emit sig_sendToMainWindow("<font color=\"#00cc00\">FlameGraph Opened  </font>");
    }else {
        emit sig_sendToMainWindow("FlameGraph open Failed !");
//        emit sig_sendToMainWindow("<font color=\"#ee0000\">Open FlameGraph Failed !</font> ");
    }
}

void Simpleperf::stopProcessor()
{
    emit stop();
}

