#include "simpleperf.h"
#include "cmd.h"


#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>


#define MY_TAG "Simpleperf"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"




Simpleperf::Simpleperf(QObject *parent):
QObject(parent)
{
    cout << QThread::currentThreadId();
    init();
}

Simpleperf::~Simpleperf()
{
    cout << " +";
    unInit();
    cout << " -";
}

/**
 * @brief Simpleperf::init
 */
void Simpleperf::init()
{
    cout;
    mSimThread = new QThread(this);
    mSimProcessor = new ProcessorImpl();
    init_connect();
    mSimProcessor->moveToThread(mSimThread);
    mSimThread->start();
    emit create();
}

/**
 * @brief Simpleperf::unInit
 */
void Simpleperf::unInit()
{
   cout;
   mSimThread->exit(0);
   delete mSimProcessor;
   delete mSimThread;
}

/**
 * @brief Simpleperf::init_connect
 */
void Simpleperf::init_connect()
{
    cout;
    connect(this,SIGNAL(create()),
            mSimProcessor,SLOT(create()));
    connect(this,SIGNAL(start()),
            mSimProcessor,SLOT(start()));
    connect(this,SIGNAL(process(QString)),
            mSimProcessor,SLOT(process(QString)));
//    qRegisterMetaType<ptrFunc>("ptrFunc");
//    connect(this,SIGNAL(process(QString,ptrFunc)),
//            mSimProcessor,SLOT(process(QString,ptrFunc)));
    connect(this,SIGNAL(stop()),
            mSimProcessor,SLOT(stop()));
    connect(this,SIGNAL(kill()),
            mSimProcessor,SLOT(kill()));
    connect(this,SIGNAL(exit()),
            mSimProcessor,SLOT(exit()));

    connect(mSimProcessor,SIGNAL(onSubmitOutput(QString)),
            this,SLOT(onReciveOutput(QString)));
    connect(mSimProcessor,SIGNAL(onSubmitError(QString)),
            this,SLOT(onReciveError(QString)));
    connect(mSimProcessor,SIGNAL(onSubmitInfo(QString)),
            this,SLOT(onReciveInfo(QString)));
    connect(mSimProcessor,SIGNAL(onSubmitState(QProcess::ProcessState)),
            this,SLOT(onReciveState(QProcess::ProcessState)));
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(mSimProcessor,SIGNAL(onSubmitExitStatus(QProcess::ExitStatus)),
            this,SLOT(onReciveExitStatus(QProcess::ExitStatus)));


}

/**
 * @brief Simpleperf::runList
 */
void Simpleperf::runList()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit onSubmitInfo("please wait!");
    run(PERFIX LIST);
}

/**
 * @brief Simpleperf::runStat
 */
void Simpleperf::runStat()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit onSubmitInfo("please wait!");
    run(QString(PERFIX STAT));
}

/**
 * @brief Simpleperf::runStat
 * @param statParams
 */
void Simpleperf::runStat(std::map<QString, QString> *statParams)
{
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit onSubmitInfo("please wait!");
    QString cmd = PERFIX "stat";
    std::map<QString, QString>::iterator it;
    for (it=statParams->begin();it!=statParams->end();it++)
    {
        if(!it->second.isEmpty()) cmd += it->second;
    }
//    cout << cmd;
    run(cmd);
}

/**
 * @brief Simpleperf::runRecord
 */
void Simpleperf::runRecord()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit onSubmitInfo("please wait!");
    emit onSubmitInfo(mSimProcessor->mUserName+RECORD);

    QString cmd1 = "adb shell rm data/local/tmp/perf.data";
    QString cmd2 = PERFIX RECORD;
    QString cmd3 = "adb pull data/local/tmp/perf.data " SIMPLEPERFDATAPATH;
    run(cmd1+";"+cmd2+";"+cmd3);

}

/**
 * @brief Simpleperf::runRecord
 * @param recordParams
 */
void Simpleperf::runRecord(std::map<QString, QString> *recordParams)
{
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit onSubmitInfo("please wait!");
    QString cmd = PERFIX " record ";
    std::map<QString, QString>::iterator it;
    for (it=recordParams->begin();it!=recordParams->end();it++){
        if(!it->second.isEmpty()) cmd += it->second;
    }
//    cout << cmd;
    run(cmd);
}

/**
 * @brief Simpleperf::runReport
 */
void Simpleperf::runReport()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit onSubmitInfo("please wait!");
    run(REPORT);
}


/**
 * @brief Simpleperf::runFlamegraph
 */
void Simpleperf::runFlamegraph()
{
    cout;
    if (m_state != QProcess::ProcessState::NotRunning)
        return emit onSubmitInfo("please wait!");
//    QString cmd1 = "FlameGraph/stackcollapse-perf.pl out.perf > out.folded";
//    QString cmd2 = "FlameGraph/flamegraph.pl out.folded > graph.svg";
//    run(cmd1+";"+cmd2);
    run(FLAMEGRAPH);
    if(QDesktopServices::openUrl(QUrl("simpleperf_data/graph.svg"))){
        emit onSubmitInfo("FlameGraph Opened");
    }else {
        emit onSubmitInfo("FlameGraph open Failed !");
    }
}

/**
 * @brief Simpleperf::runQuickGeneration
 */
void Simpleperf::runQuickGeneration()
{
    cout;
//    process();
}

/**
 * @brief Simpleperf::run 执行Simpleperf命令
 */
void Simpleperf::run(QString cmd)
{
    if(!cmd.isEmpty() && mSimProcessor->getState() == ProcessState::NotRunning){
        emit process(cmd);
        emit onSubmitInfo(color.GREEN.arg(mSimProcessor->mUserName)+cmd);
    }else{
        emit onSubmitInfo("Warning : command is empty or process is running!");
    }
}


/**
 * @brief Simpleperf::terminal
 */
void Simpleperf::terminal()
{
    cout;
    if(mSimProcessor->getState() != ProcessState::NotRunning){
        emit stop();
    }
}

/**
 * @brief Simpleperf::onReciveOutput
 * @param output
 */
void Simpleperf::onReciveOutput(QString output)
{
    cout << output;
    emit onSubmitOutput(output);
}

/**
 * @brief Simpleperf::onReciveError
 * @param error
 */
void Simpleperf::onReciveError(QString error)
{
    cout << error;
    emit onSubmitError(error);
}

/**
 * @brief Simpleperf::onReciveInfo
 * @param info
 */
void Simpleperf::onReciveInfo(QString info)
{
    cout << info;
    emit onSubmitInfo(info);
}

/**
 * @brief Simpleperf::onReciveStatus
 * @param state
 */
void Simpleperf::onReciveState(QProcess::ProcessState state)
{
    cout << state;
    emit onSubmitState(SIMPLEPERF,state);
}

/**
 * @brief Simpleperf::onReciveExitStatus
 * @param exitStatus
 */
void Simpleperf::onReciveExitStatus(QProcess::ExitStatus exitStatus)
{
    cout << exitStatus;
    emit onSubmitExitStatus(SIMPLEPERF,exitStatus);
}
