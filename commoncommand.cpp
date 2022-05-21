#include "commoncommand.h"

#include <QDebug>

#define MY_TAG "CommonCommand"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

/**
 * @brief CommonCommand::CommonCommand
 * @param parent
 */
CommonCommand::CommonCommand(QObject *parent) : QObject(parent)
{
    cout <<QThread::currentThreadId();
    init();
}

/**
 * @brief CommonCommand::~CommonCommand
 */
CommonCommand::~CommonCommand()
{
    cout ;
    unInit();
}

/**
 * @brief CommonCommand::init
 */
void CommonCommand::init()
{
    cout;
    mCcdThread = new QThread(this);
    mCcdProcessor = new ProcessorImpl();
    init_connect();
    mCcdProcessor->moveToThread(mCcdThread);
    mCcdThread->start();
    emit create();
}

/**
 * @brief CommonCommand::unInit
 */
void CommonCommand::unInit()
{
   cout;
   mCcdThread->exit(0);
   delete mCcdProcessor;
   delete mCcdThread;
}

/**
 * @brief CommonCommand::init_connect
 */
void CommonCommand::init_connect()
{
//    connect(this,SIGNAL(start()),
//            mCcdProcessor,SLOT(createProcessor()));
//    connect(this,SIGNAL(processCommand(QString)),
//            mCcdProcessor,SLOT(process(QString)));
//    connect(mCcdProcessor,SIGNAL(sig_sendOutput(QString)),
//            this,SLOT(slo_reciveOutput(QString)));
//    connect(this,SIGNAL(stop()),
//            mCcdProcessor,SLOT(stopProcessor()));
//    connect(mCcdProcessor,SIGNAL(sig_sendError(QString)),
//            this,SLOT(slo_reciveError(QString)));
//    connect(mCcdProcessor,SIGNAL(sig_sendInfo(QString)),
//            this,SLOT(slo_reciveInfo(QString)));
//    connect(mCcdProcessor,SIGNAL(sig_sendState(QProcess::ProcessState)),
//            this,SLOT(slo_reciveState(QProcess::ProcessState)));
    cout;
    connect(this,SIGNAL(create()),
            mCcdProcessor,SLOT(create()));
    connect(this,SIGNAL(start()),
            mCcdProcessor,SLOT(start()));
    connect(this,SIGNAL(process(QString)),
            mCcdProcessor,SLOT(process(QString)));
    qRegisterMetaType<ptrFunc>("ptrFunc");
    connect(this,SIGNAL(process(QString,ptrFunc)),
            mCcdProcessor,SLOT(process(QString,ptrFunc)));
    connect(this,SIGNAL(stop()),
            mCcdProcessor,SLOT(stop()));
    connect(this,SIGNAL(kill()),
            mCcdProcessor,SLOT(kill()));
    connect(this,SIGNAL(exit()),
            mCcdProcessor,SLOT(exit()));

    connect(mCcdProcessor,SIGNAL(onSubmitOutput(QString)),
            this,SLOT(onReciveOutput(QString)));
    connect(mCcdProcessor,SIGNAL(onSubmitError(QString)),
            this,SLOT(onReciveError(QString)));
    connect(mCcdProcessor,SIGNAL(onSubmitInfo(QString)),
            this,SLOT(onReciveInfo(QString)));
    connect(mCcdProcessor,SIGNAL(onSubmitState(QProcess::ProcessState)),
            this,SLOT(onReciveState(QProcess::ProcessState)));
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(mCcdProcessor,SIGNAL(onSubmitExitStatus(QProcess::ExitStatus)),
            this,SLOT(onReciveExitStatus(QProcess::ExitStatus)));
}

/**
 * @brief CommonCommand::run 执行ssh命令
 */
void CommonCommand::run(QString cmd)
{
    if(!cmd.isEmpty() && mCcdProcessor->getState() == ProcessState::NotRunning){
        emit process(cmd);
        onSubmitInfo(color.GREEN.arg(mCcdProcessor->mUserName)+cmd);
    }else{
        onSubmitInfo("Warning : command is empty or process is running!");
    }
}

/**
 * @brief CommonCommand::terminal
 */
void CommonCommand::terminal()
{
    cout;
    if(mCcdProcessor->getState() != ProcessState::NotRunning){
        emit stop();
    }
}

/**
 * @brief CommonCommand::onReciveOutput
 * @param output
 */
void CommonCommand::onReciveOutput(QString output)
{
    cout << output;
    emit onSubmitOutput(output);
}

/**
 * @brief CommonCommand::onReciveError
 * @param error
 */
void CommonCommand::onReciveError(QString error)
{
    cout << error;
    emit onSubmitError(error);
}

/**
 * @brief CommonCommand::onReciveInfo
 * @param info
 */
void CommonCommand::onReciveInfo(QString info)
{
    cout << info;
    emit onSubmitInfo(info);
}

/**
 * @brief CommonCommand::onReciveStatus
 * @param state
 */
void CommonCommand::onReciveState(QProcess::ProcessState state)
{
    cout << state;
    emit onSubmitState(COMMAND,state);
}

/**
 * @brief CommonCommand::onReciveExitStatus
 * @param exitStatus
 */
void CommonCommand::onReciveExitStatus(QProcess::ExitStatus exitStatus)
{
    cout << exitStatus;
    emit onSubmitExitStatus(COMMAND,exitStatus);
}
