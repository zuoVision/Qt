#include "ProcessorImpl.h"


#include <QDebug>
#include <QThread>

#define MY_TAG "ProcessorImpl"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

#define BASH "bash"
#define CMD  QStringList() << "-c"


/**
 * @brief ProcessorImpl::ProcessorImpl
 * @param parent
 */
ProcessorImpl::ProcessorImpl(QObject *parent) : QObject(parent)
{
    cout;
    init();
}

/**
 * @brief ProcessorImpl::~ProcessorImpl
 */
ProcessorImpl::~ProcessorImpl()
{
    cout;
}

/**
 * @brief ProcessorImpl::init
 */
void ProcessorImpl::init()
{
    cout;
    mProp.id = 0x0000;
    mProp.name = "test";
    mProp.status = ProcessState::NotRunning;
}

/**
 * @brief ProcessorImpl::uninit
 */
void ProcessorImpl::uninit()
{
    cout;
}

/**
 * @brief ProcessorImpl::initConnect
 * @param processor
 */
void ProcessorImpl::initConnect(QProcess *processor)
{
    connect(processor,SIGNAL(readyReadStandardOutput()),
            this,SLOT(onOutputListener()));
    connect(processor,SIGNAL(readyReadStandardError()),
            this,SLOT(onErrorListener()));
    qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
    connect(processor,SIGNAL(stateChanged(QProcess::ProcessState)),
            this,SLOT(onStatusListener(QProcess::ProcessState)));
}

/**
 * @brief ProcessorImpl::create
 */
void ProcessorImpl::create()
{
    cout << QThread::currentThreadId();
    mProcessor = new QProcess();
    start();
    initConnect(mProcessor);

}

/**
 * @brief ProcessorImpl::start
 */
void ProcessorImpl::start()
{
    cout;
    QStringList cmd = QStringList() << "-c" << "whoami;hostname";
    cout << cmd;
    mProcessor->start(BASH,cmd);
    mProcessor->waitForFinished();
    QString userName = mProcessor->readAllStandardOutput();
    cout << userName;
//    getUserName(mProcessor->readAllStandardOutput());
}

/**
 * @brief ProcessorImpl::process
 * @param cmd
 */
void ProcessorImpl::process(QString cmd)
{
    cout << " + ";

    cout << "processing...\n" << cmd;


    cout << " - ";
}

/**
 * @brief ProcessorImpl::stop
 */
void ProcessorImpl::stop()
{
    cout;
    mProcessor->close();
}

/**
 * @brief ProcessorImpl::kill
 */
void ProcessorImpl::kill()
{
    cout;
    if(mProcessor->state() != ProcessState::NotRunning){
        mProcessor->kill();
    }
}

/**
 * @brief ProcessorImpl::exit
 */
void ProcessorImpl::exit()
{
    cout << mProcessor->exitStatus();
    mProcessor->terminate();
    delete mProcessor;
}

/**
 * @brief ProcessorImpl::getUserName
 * @param processor
 */
void ProcessorImpl::getUserName(QString userName)
{
    cout << userName;
    QStringList name = userName.split("\n");
    if(!name.isEmpty()) name.pop_back();
    mUserName = name.first().append("@").append(name.last()).append(":~$ ");
    cout << mUserName;
}

/**
 * @brief ProcessorImpl::onOutputListener
 */
void ProcessorImpl::onOutputListener()
{
    cout << mProcessor->readAllStandardOutput();
}

/**
 * @brief ProcessorImpl::onErrorListener
 */
void ProcessorImpl::onErrorListener()
{
    cout << mProcessor->readAllStandardError();
}

/**
 * @brief ProcessorImpl::onStatusListener
 * @param state
 */
void ProcessorImpl::onStatusListener(QProcess::ProcessState state)
{
    cout << state;
}
