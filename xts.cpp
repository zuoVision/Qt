#include "xts.h"
#include "cmd.h"

#include <QDataStream>
#include <QDebug>

#define MY_TAG          "Xts"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"




Xts::Xts(QObject *parent) : QObject(parent)
{
    cout << QThread::currentThreadId();
    init();
}

Xts::~Xts()
{
    cout <<"~Xts() +";
    unInit();
    cout<<"~Xts() -";
}

void Xts::init()
{
    mXtsThread = new QThread(this);
    mXtsProcessor = new ProcessorImpl();
    init_connect();
    mXtsProcessor->moveToThread(mXtsThread);
    mXtsThread->start();
    emit create();
}

/**
 * @brief Xts::unInit
 * //TODO:QThread: Destroyed while thread is still running
 *          mXtsThread->exit(0);
 *          delete mXtsProcessor;
 *          delete mXtsThread;
 */
void Xts::unInit()
{
    cout;
    mXtsThread->exit(0);
    delete mXtsProcessor;
    delete mXtsThread;
}

void Xts::init_connect()
{
//    connect(this,SIGNAL(start()),
//            mXtsProcessor,SLOT(createProcessor()));
//    connect(this,SIGNAL(processCommand(QString)),
//            mXtsProcessor,SLOT(process(QString)));
//    connect(this,SIGNAL(processCommand(QString,QString *)),
//            mXtsProcessor,SLOT(process(QString,QString *)));
//    connect(this,SIGNAL(stop()),
//            mXtsProcessor,SLOT(stopProcessor()));
//    connect(this,SIGNAL(exit()),
//            mXtsProcessor,SLOT(exitProcessor()));
//    connect(mXtsProcessor,SIGNAL(sig_sendOutput(QString)),
//            this,SLOT(slo_reciveOutput(QString)));
//    connect(mXtsProcessor,SIGNAL(sig_sendError(QString)),
//            this,SLOT(slo_reciveError(QString)));
//    connect(mXtsProcessor,SIGNAL(sig_sendInfo(QString)),
//            this,SLOT(slo_reciveInfo(QString)));
//    connect(mXtsProcessor,SIGNAL(sig_sendState(QProcess::ProcessState)),
//            this,SLOT(slo_reciveState(QProcess::ProcessState)));

    connect(this,SIGNAL(create()),
            mXtsProcessor,SLOT(create()));
    connect(this,SIGNAL(start()),
            mXtsProcessor,SLOT(start()));
    connect(this,SIGNAL(process(QString)),
            mXtsProcessor,SLOT(process(QString)));
    qRegisterMetaType<ptrFunc>("ptrFunc");
    connect(this,SIGNAL(process(QString,ptrFunc)),
            mXtsProcessor,SLOT(process(QString,ptrFunc)));
    connect(this,SIGNAL(stop()),
            mXtsProcessor,SLOT(stop()));
    connect(this,SIGNAL(kill()),
            mXtsProcessor,SLOT(kill()));
    connect(this,SIGNAL(exit()),
            mXtsProcessor,SLOT(exit()));

    connect(mXtsProcessor,SIGNAL(onSubmitOutput(QString)),
            this,SLOT(onReciveOutput(QString)));
    connect(mXtsProcessor,SIGNAL(onSubmitError(QString)),
            this,SLOT(onReciveError(QString)));
    connect(mXtsProcessor,SIGNAL(onSubmitInfo(QString)),
            this,SLOT(onReciveInfo(QString)));
    connect(mXtsProcessor,SIGNAL(onSubmitState(QProcess::ProcessState)),
            this,SLOT(onReciveState(QProcess::ProcessState)));
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(mXtsProcessor,SIGNAL(onSubmitExitStatus(QProcess::ExitStatus)),
            this,SLOT(onReciveExitStatus(QProcess::ExitStatus)));

}


//void Xts::slo_reciveOutput(QString output)
//{
//    cout << output;
//    if(output.contains("CommandScheduler: All done")) //"SuiteResultReporter: "
//        stopProcessor();
//        m_ctsComplete = true;
//    if (m_flag && output.contains("cts-tradefed")) {
//        QStringList res = output.split("\n").filter("cts-tradefed");
//        if(res.isEmpty()){
//            emit sig_sendToMainWindow("Warning : No suite detected, please reselect cts suite!");
//            m_flag = false;
//            return;
//        }
//        if(res.size()>1){
//            emit sig_sendToMainWindow("Warning : Multiple suite detected, please reselect cts suite!");
//            m_flag = false;
//            m_ctsSuite.clear();
//            return;
//        }
//        m_ctsSuite = res.first();
//        return;
//    }
//    emit sig_sendToMainWindow(output);
//}

//void Xts::slo_reciveError(QString error)
//{
////    cout << error;
//    emit sig_sendToMainWindow(error);
//}

//void Xts::slo_reciveInfo(QString info)
//{
////    cout << info;
//    emit sig_sendToMainWindow(info);
//}

//void Xts::slo_reciveState(QProcess::ProcessState state)
//{
//    cout << state;
//    emit sig_sendToMainWindow(state,MY_TAG);
//    if (state==QProcess::ProcessState::NotRunning)
//    {
//        if(!m_flag)
//            emit sig_sendToMainWindow("Done");
//        if(m_ctsComplete){
//            if (m_ctsCommand.contains("run")) emit sig_showCtsResult();
//            m_ctsComplete=false;
//        }
//    }
//}

//void Xts::slo_reciveMainWindow(QString msg)
//{
//    m_ctsSuite.clear();
//    processCommand(QString("find %1 -name cts-tradefed").arg(msg));
//    m_flag = true;
//}

//void Xts::runCts(const QString arg1, const QString arg2, QString arg3, QString arg4)
//{
//    //arg1 deprecated
//    cout << arg1 <<arg2 <<arg3 <<arg4;
//    if (mXtsProcessor->processor->state() != QProcess::ProcessState::NotRunning)
//        return emit sig_sendToMainWindow("please wait!");
//    m_ctsCommand = arg2;
//    if(!arg3.isEmpty()) {
//        arg3 = QString(" -m %1").arg(arg3);
//    }
//    if(!arg4.isEmpty()){
//        arg4 = QString(" -t %1").arg(arg4);
//        cout << arg4;
//    }
//    QString cmd = m_ctsSuite +" "+arg2+arg3+arg4;
//    cout << cmd;
//    emit sig_sendToMainWindow(mXtsProcessor->m_userName+cmd);
//    emit processCommand(cmd);
//}

//void Xts::stopProcessor()
//{
//    emit stop();
//}


/**
 * @brief Xts::run
 * @param arg1 : cts suite
 * @param arg2 : cts command
 * @param arg3 : cts module
 * @param arg4 : cts test
 */
void Xts::run(const QString arg1, const QString arg2, QString arg3, QString arg4)
{
        //arg1 deprecated
        cout << arg1 <<arg2 <<arg3 <<arg4;
        if (mXtsProcessor->getState() != ProcessState::NotRunning)
            return emit onSubmitInfo("please wait!");
        m_ctsCommand = arg2;
        if(!arg3.isEmpty()) {
            arg3 = QString(" -m %1").arg(arg3);
        }
        if(!arg4.isEmpty()){
            arg4 = QString(" -t %1").arg(arg4);
            cout << arg4;
        }
        QString cmd = m_ctsSuite +" "+arg2+arg3+arg4;
        cout << cmd;
        emit onSubmitInfo(mXtsProcessor->mUserName+cmd);
        emit process(cmd);
}

/**
 * @brief Xts::terminal
 */
void Xts::terminal()
{
    cout;
    if(mXtsProcessor->getState() != ProcessState::NotRunning){
        emit stop();
    }
}

/**
 * @brief Xts::onReciveOutput
 * @param output
 */
void Xts::onReciveOutput(QString output)
{
    cout << output;
    if(output.contains("CommandScheduler: All done")){ //"SuiteResultReporter: "
        emit stop();
        m_ctsComplete = true;
    }
    if (m_flag && output.contains("cts-tradefed")) {
        QStringList res = output.split("\n").filter("cts-tradefed");
        if(res.isEmpty()){
            emit onSubmitOutput("Warning : No suite detected, please reselect cts suite!");
            m_flag = false;
            return;
        }
        if(res.size()>1){
            emit onSubmitOutput("Warning : Multiple suite detected, please reselect cts suite!");
            m_flag = false;
            m_ctsSuite.clear();
            return;
        }
        m_ctsSuite = res.first();
        return;
    }
    emit onSubmitOutput(output);
}

/**
 * @brief Xts::onReciveError
 * @param error
 */
void Xts::onReciveError(QString error)
{
    cout << error;
    emit onSubmitError(error);
}

/**
 * @brief Xts::onReciveInfo
 * @param info
 */
void Xts::onReciveInfo(QString info)
{
    cout << info;
    emit onSubmitInfo(info);
}

/**
 * @brief Xts::onReciveStatus
 * @param state
 */
void Xts::onReciveState(QProcess::ProcessState state)
{
    cout << state;
    emit onSubmitState(XTS,state);
    if (state==ProcessState::NotRunning)
    {
        if(!m_flag)
            emit onSubmitInfo("Done");
        if(m_ctsComplete){
            if (m_ctsCommand.contains("run")) emit onShowCtsResult();
            m_ctsComplete=false;
        }
    }
}

/**
 * @brief Xts::onReciveExitStatus
 * @param exitStatus
 */
void Xts::onReciveExitStatus(QProcess::ExitStatus exitStatus)
{
    cout << exitStatus;
    emit onSubmitExitStatus(XTS,exitStatus);
}


