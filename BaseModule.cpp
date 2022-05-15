#include "BaseModule.h"
#include "utils/general/general.h"

#include <QDebug>
#include <qthread.h>

#define MY_TAG          "BaseModule"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

/**
 * @brief BaseModule::BaseModule
 * @param parent
 */
BaseModule::BaseModule(QObject *parent) : QObject(parent)
{
    cout;
//    init();
}

/**
 * @brief BaseModule::~BaseModule
 */
BaseModule::~BaseModule()
{
    cout;
}

/**
 * @brief BaseModule::init
 */
void BaseModule::init()
{
    cout;
//    mBaseModuleThread = new QThread(this);
//    mBaseModuleProcessor = new ProcessorImpl();
//    init_connect();
//    mBaseModuleProcessor->moveToThread(mBaseModuleThread);
//    mBaseModuleThread->start();
//    emit create();
}

/**
 * @brief BaseModule::unInit
 */
void BaseModule::unInit()
{
    cout;
//    delete mBaseModuleProcessor;
//    delete mBaseModuleThread;
}

/**
 * @brief BaseModule::init_connect
 */
void BaseModule::init_connect()
{
    cout;
    connect(this,SIGNAL(create()),
            mBaseModuleProcessor,SLOT(create()));
    connect(this,SIGNAL(start()),
            mBaseModuleProcessor,SLOT(start()));
    connect(this,SIGNAL(process(QString)),
            mBaseModuleProcessor,SLOT(process(QString)));
    connect(this,SIGNAL(stop()),
            mBaseModuleProcessor,SLOT(stop()));
    connect(this,SIGNAL(kill()),
            mBaseModuleProcessor,SLOT(kill()));
    connect(this,SIGNAL(exit()),
            mBaseModuleProcessor,SLOT(exit()));

    connect(mBaseModuleProcessor,SIGNAL(onSubmitOutput(QString)),
            this,SLOT(onReciveOutput(QString)));
    connect(mBaseModuleProcessor,SIGNAL(onSubmitError(QString)),
            this,SLOT(onReciveError(QString)));
    connect(mBaseModuleProcessor,SIGNAL(onSubmitInfo(QString)),
            this,SLOT(onReciveInfo(QString)));
    connect(mBaseModuleProcessor,SIGNAL(onSubmitState(QProcess::ProcessState)),
            this,SLOT(onReciveState(QProcess::ProcessState)));
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(mBaseModuleProcessor,SIGNAL(onSubmitExitStatus(QProcess::ExitStatus)),
            this,SLOT(onReciveExitStatus(QProcess::ExitStatus)));
}

/**
 * @brief BaseModule::login
 */
void BaseModule::login()
{
    cout ;

    if(mBaseModuleProcessor->getState() == ProcessState::NotRunning){
        emit process("ping baidu.com");
    }else {
        emit onSubmitInfo("急什么！");
    }
}

/**
 * @brief BaseModule::logout
 */
void BaseModule::logout()
{
    cout;
    emit stop();
}

/**
 * @brief BaseModule::onReciveOutput
 * @param output
 */
void BaseModule::onReciveOutput(QString output)
{
    cout << output;
    emit onSubmitOutput(output);
}

/**
 * @brief BaseModule::onReciveError
 * @param error
 */
void BaseModule::onReciveError(QString error)
{
    cout << error;
    emit onSubmitError(error);
}

/**
 * @brief BaseModule::onReciveInfo
 * @param info
 */
void BaseModule::onReciveInfo(QString info)
{
    cout << info;
    emit onSubmitInfo(info);
}

/**
 * @brief BaseModule::onReciveStatus
 * @param state
 */
void BaseModule::onReciveState(QProcess::ProcessState state)
{
    cout << state;
    emit onSubmitState(3,state);
}

/**
 * @brief BaseModule::onReciveExitStatus
 * @param exitStatus
 */
void BaseModule::onReciveExitStatus(QProcess::ExitStatus exitStatus)
{
    cout << exitStatus;
    emit onSubmitExitStatus(3,exitStatus);
}



