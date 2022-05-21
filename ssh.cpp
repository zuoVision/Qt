#include "ssh.h"
#include "cmd.h"

#include <QDebug>
#include <qthread.h>

#define MY_TAG          "Ssh"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

/**
 * @brief Ssh::Ssh
 * @param parent
 */
Ssh::Ssh(QObject *parent) : QObject(parent)
{
    cout;
    init();
}

/**
 * @brief Ssh::~Ssh
 */
Ssh::~Ssh()
{
    cout;
    unInit();
}

/**
 * @brief Ssh::init
 */
void Ssh::init()
{
    cout;
    mSshThread = new QThread(this);
    mSshProcessor = new ProcessorImpl();
    init_connect();
    mSshProcessor->moveToThread(mSshThread);
    mSshThread->start();
    emit create();
}

/**
 * @brief Ssh::unInit
 */
void Ssh::unInit()
{
    cout;
    mSshThread->exit(0);
    delete mSshProcessor;
    delete mSshThread;
}

/**
 * @brief Ssh::init_connect
 */
void Ssh::init_connect()
{
    cout;
    connect(this,SIGNAL(create()),
            mSshProcessor,SLOT(create()));
    connect(this,SIGNAL(start()),
            mSshProcessor,SLOT(start()));
    connect(this,SIGNAL(process(QString)),
            mSshProcessor,SLOT(process(QString)));
    qRegisterMetaType<ptrFunc>("ptrFunc");
    connect(this,SIGNAL(process(QString,ptrFunc)),
            mSshProcessor,SLOT(process(QString,ptrFunc)));
    connect(this,SIGNAL(stop()),
            mSshProcessor,SLOT(stop()));
    connect(this,SIGNAL(kill()),
            mSshProcessor,SLOT(kill()));
    connect(this,SIGNAL(exit()),
            mSshProcessor,SLOT(exit()));

    connect(mSshProcessor,SIGNAL(onSubmitOutput(QString)),
            this,SLOT(onReciveOutput(QString)));
    connect(mSshProcessor,SIGNAL(onSubmitError(QString)),
            this,SLOT(onReciveError(QString)));
    connect(mSshProcessor,SIGNAL(onSubmitInfo(QString)),
            this,SLOT(onReciveInfo(QString)));
    connect(mSshProcessor,SIGNAL(onSubmitState(QProcess::ProcessState)),
            this,SLOT(onReciveState(QProcess::ProcessState)));
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(mSshProcessor,SIGNAL(onSubmitExitStatus(QProcess::ExitStatus)),
            this,SLOT(onReciveExitStatus(QProcess::ExitStatus)));
}

void Ssh::login(QString addr,ptrFunc cb)
{
    cout ;

    if(mSshProcessor->getState() == ProcessState::NotRunning){
        emit process(addr,cb);
        emit onSubmitInfo(color.GREEN.arg(mSshProcessor->mUserName));
    }else {
        emit onSubmitInfo("please wait!");
    }

}

/**
 * @brief Ssh::logout
 */
void Ssh::logout()
{
    cout;
    if(mSshProcessor->getState() != ProcessState::NotRunning){
        emit stop();
    }
}

/**
 * @brief Ssh::run 执行ssh命令
 */
void Ssh::run(QString cmd)
{
    if(!cmd.isEmpty() && mSshProcessor->getState() == ProcessState::NotRunning){
        emit process(cmd);
        onSubmitInfo(color.GREEN.arg(mSshProcessor->mUserName)+cmd);
    }else{
        onSubmitInfo("Warning : command is empty or process is running!");
    }
}

/**
 * @brief Ssh::terminal
 */
void Ssh::terminal()
{
    cout;
}

/**
 * @brief Ssh::onReciveOutput
 * @param output
 */
void Ssh::onReciveOutput(QString output)
{
    cout << output;
    emit onSubmitOutput(output);
}

/**
 * @brief Ssh::onReciveError
 * @param error
 */
void Ssh::onReciveError(QString error)
{
    cout << error;
    emit onSubmitError(error);
}

/**
 * @brief Ssh::onReciveInfo
 * @param info
 */
void Ssh::onReciveInfo(QString info)
{
    cout << info;
    emit onSubmitInfo(info);
}

/**
 * @brief Ssh::onReciveStatus
 * @param state
 */
void Ssh::onReciveState(QProcess::ProcessState state)
{
    cout << state;
    emit onSubmitState(SSH,state);
}

/**
 * @brief Ssh::onReciveExitStatus
 * @param exitStatus
 */
void Ssh::onReciveExitStatus(QProcess::ExitStatus exitStatus)
{
    cout << exitStatus;
    emit onSubmitExitStatus(SSH,exitStatus);
}



