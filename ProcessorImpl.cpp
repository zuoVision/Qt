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
ProcessorImpl::ProcessorImpl(QObject *parent) :
    QObject(parent),
    mUserName("NANANA"),
    mState(ProcessState::NotRunning)
{
    cout << QThread::currentThreadId();
    init();
}

/**
 * @brief ProcessorImpl::~ProcessorImpl
 */
ProcessorImpl::~ProcessorImpl()
{
    cout;
    uninit();
}

/**
 * @brief ProcessorImpl::init
 */
void ProcessorImpl::init()
{
    cout;
//    mProp.id = 0x0000;
//    mProp.name = "test";
//    mProp.status = ProcessState::NotRunning;

}

/**
 * @brief ProcessorImpl::uninit
 */
void ProcessorImpl::uninit()
{
    cout;
    if(mProcessor){
        cout << "delete mProcessor";
        delete mProcessor;
    }
}

/**
 * @brief ProcessorImpl::initConnect
 * @param processor
 */
void ProcessorImpl::initConnect()
{
    cout;
    connect(mProcessor,SIGNAL(finished(int,QProcess::ExitStatus)),
            this,SLOT(onFinishedListener(int,QProcess::ExitStatus)));
    connect(mProcessor,SIGNAL(readyReadStandardOutput()),
            this,SLOT(onOutputListener()));
    connect(mProcessor,SIGNAL(readyReadStandardError()),
            this,SLOT(onErrorListener()));
    qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
    connect(mProcessor,SIGNAL(stateChanged(QProcess::ProcessState)),
            this,SLOT(onStateListener(QProcess::ProcessState)));
}

/**
 * @brief ProcessorImpl::create
 */
void ProcessorImpl::create()
{
    cout << QThread::currentThreadId();
    mProcessor = new QProcess();
    start();
    initConnect();
}

/**
 * @brief ProcessorImpl::start
 */
void ProcessorImpl::start()
{
    cout;
    QStringList cmd = QStringList() << "-c" << "whoami;hostname";
    mProcessor->start(BASH,cmd);
    mProcessor->waitForFinished();
    QString userName = mProcessor->readAllStandardOutput();
    setUserName(userName);
//    emit onSubmitInfo("start");
}

/**
 * @brief ProcessorImpl::process
 * @param cmd
 */
//void ProcessorImpl::process(QString cmd)
//{
//    cout << " + ";
//    mProcessor->start(BASH,CMD << cmd);
//    mProcessor->waitForReadyRead();
//    cout << " - ";
//}

void ProcessorImpl::process(QString cmd,callbcakFunc cbf)
{
    cout << " + ";
    mCbf = cbf;
    mProcessor->start(BASH,CMD << cmd);
    mProcessor->waitForReadyRead();
    cout << " - ";

}

/**
 * @brief ProcessorImpl::stop
 */
void ProcessorImpl::stop()
{
    cout;
    mProcessor->close();
    emit onSubmitInfo("stop");
}

/**
 * @brief ProcessorImpl::kill
 */
void ProcessorImpl::kill()
{
    cout;
    if(mProcessor->state() != ProcessState::NotRunning){
        mProcessor->kill();
        emit onSubmitInfo("kill");
    }

}

/**
 * @brief ProcessorImpl::exit
 */
void ProcessorImpl::exit()
{
    cout << mProcessor->exitStatus();
    mProcessor->terminate();
    emit onSubmitInfo("exit");
}

/**
 * @brief ProcessorImpl::getUserName
 * @param processor
 */
void ProcessorImpl::setUserName(QString userName)
{
    cout << userName;
    QStringList name = userName.split("\n");
    if(!name.isEmpty()) name.pop_back();
    mUserName = name.first().append("@").append(name.last()).append(":~$ ");
    cout << mUserName;
//    emit onSubmitInfo(mUserName);
}

/**
 * @brief ProcessorImpl::getUserName
 * @return
 */
QString ProcessorImpl::getUserName()
{
    return mUserName;
}

/**
 * @brief ProcessorImpl::setState
 * @param state
 */
void ProcessorImpl::setState(QProcess::ProcessState state)
{
    mState = state;
}

/**
 * @brief ProcessorImpl::getState
 * @return
 */
ProcessorImpl::ProcessState ProcessorImpl::getState()
{
    return mState;
}

void ProcessorImpl::onHandleCallback()
{
    cout;
    if(mCbf != nullptr) {
        mCbf(CallbackState::Error);
        mCbf=nullptr;
    }
}

/**
 * @brief ProcessorImpl::onOutputListener
 */
void ProcessorImpl::onOutputListener()
{
    mOutput = mProcessor->readAllStandardOutput();
//    cout << mOutput;
    //remove endwith "\n"
    mOutput.replace(QRegExp("\n$"), "");
    emit onSubmitOutput(mOutput);
}

/**
 * @brief ProcessorImpl::onErrorListener
 */
void ProcessorImpl::onErrorListener()
{
    mError = mProcessor->readAllStandardError();
    cout << mError;
    //remove endwith "\n"
    mError.replace(QRegExp("\n$"), "");
    emit onSubmitError(mError);
    onHandleCallback();
}

/**
 * @brief ProcessorImpl::onStatusListener
 * @param state
 */
void ProcessorImpl::onStateListener(QProcess::ProcessState state)
{
    cout << state;
    setState(state);
    emit onSubmitState(state);
}

/**
 * @brief ProcessorImpl::onFinishedListener
 */
void ProcessorImpl::onFinishedListener(int exitCode, QProcess::ExitStatus exitStatus)
{
   cout << exitCode << exitStatus;
   emit onSubmitExitStatus(exitStatus);
}
