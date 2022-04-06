#include "simpleperf.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>


#define MY_TAG "Simpleperf"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]"

#define PERFIX "adb shell data/local/tmp/simpleperf "
#define LIST   "list"
#define STAT   "stat -e cache-references,cache-misses -a --duration 10 "
#define RECORD "record -o /data/local/tmp/perf.data -e task-clock:u -f 1000 -g --duration 10 --log info --app com.tcl.camera"
#define REPORT "python scripts/report_sample.py > out.perf"


Simpleperf::Simpleperf(QObject *parent):
QObject(parent)
{
    cout << QThread::currentThreadId();
    init();
    sim_Thread = new QThread(this);
    sim_cpt = new CommandProcessThread();
    init_connect();
    sim_cpt->moveToThread(sim_Thread);
    sim_Thread->start();
    emit start();

}

Simpleperf::~Simpleperf()
{
    cout << " +";
    sim_Thread->exit(0);
    delete sim_cpt;
    delete sim_Thread;
    cout << " -";
}

void Simpleperf::init()
{
    m_hostStyle = QString("<font color=\'#006400\'>%1 </font>");
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
//    cout;
    emit sig_sendToMainWindow(output);
}

void Simpleperf::slo_reciveError(QString error)
{
//    cout << "slo_reciveError" << error;
    emit sig_sendToMainWindow(error);
}

void Simpleperf::slo_reciveInfo(QString info)
{
//    cout << "slo_reciveInfo" << info;
    emit sig_sendToMainWindow(info);
}

void Simpleperf::slo_reciveState(QProcess::ProcessState state)
{
//    cout << "slo_reciveState" << state;
    emit sig_sendToMainWindow(state,MY_TAG);
    if (state==QProcess::ProcessState::NotRunning)
        emit sig_sendToMainWindow("Done");
    m_state = state;
}

void Simpleperf::runList()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    process(PERFIX LIST);
}

void Simpleperf::runStat()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    process(QString(PERFIX STAT));
}

void Simpleperf::runStat(std::map<QString, QString> *statParams)
{
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    QString cmd = PERFIX "stat";
    std::map<QString, QString>::iterator it;
    for (it=statParams->begin();it!=statParams->end();it++)
    {
        if(!it->second.isEmpty()) cmd += it->second;
    }
//    cout << cmd;
    process(cmd);
}

void Simpleperf::runRecord()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    emit sig_sendToMainWindow(sim_cpt->m_userName+RECORD);

    QString cmd1 = "adb shell rm /data/local/tmp/perf.data";
    QString cmd2 = PERFIX RECORD;
    QString cmd3 = "adb pull /data/local/tmp/perf.data";
    process(cmd1+";"+cmd2+";"+cmd3);

}

void Simpleperf::runRecord(std::map<QString, QString> *recordParams)
{
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    QString cmd = PERFIX " record ";
    std::map<QString, QString>::iterator it;
    for (it=recordParams->begin();it!=recordParams->end();it++){
        if(!it->second.isEmpty()) cmd += it->second;
    }
//    cout << cmd;
    process(cmd);
}

void Simpleperf::runReport()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    process(REPORT);
}


void Simpleperf::runFlamegraph()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit sig_sendToMainWindow("please wait!");
    QString cmd1 = "FlameGraph/stackcollapse-perf.pl out.perf > out.folded";
    QString cmd2 = "FlameGraph/flamegraph.pl out.folded > graph.svg";
    process(cmd1+";"+cmd2);

    if(QDesktopServices::openUrl(QUrl("graph.svg"))){
        emit sig_sendToMainWindow("FlameGraph Opened");
//        emit sig_sendToMainWindow("<font color=\"#00cc00\">FlameGraph Opened  </font>");
    }else {
        emit sig_sendToMainWindow("FlameGraph open Failed !");
//        emit sig_sendToMainWindow("<font color=\"#ee0000\">Open FlameGraph Failed !</font> ");
    }
}

void Simpleperf::runQuickGeneration()
{
    cout;
//    process();
}

void Simpleperf::process(QString cmd)
{
    cout;
    emit sig_sendToMainWindow(m_hostStyle.arg(sim_cpt->m_userName)+cmd);
    emit processCommand(cmd);
}

void Simpleperf::stopProcessor()
{
    emit stop();
}

