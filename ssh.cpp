#include "ssh.h"

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
    connect(this,SIGNAL(stop()),
            mSshProcessor,SLOT(stop()));
    connect(this,SIGNAL(kill()),
            mSshProcessor,SLOT(kill()));
    connect(this,SIGNAL(exit()),
            mSshProcessor,SLOT(exit()));

//    connect(mSshProcessor,SIGNAL(onSubmitOutput(QString)),
//            this,SLOT(onReciveOutput(QString)));
//    connect(mSshProcessor,SIGNAL(onSubmitError(QString)),
//            this,SLOT(onReciveError(QString)));
//    connect(mSshProcessor,SIGNAL(onSubmitInfo(QString)),
//            this,SLOT(onReciveInfo(QString)));
//    connect(mSshProcessor,SIGNAL(onSubmitStatus(ProcessState)),
//            this,SLOT(onReciveStatus(ProcessState)));
}

/**
 * @brief Ssh::login
 */
void Ssh::login()
{
    cout;
    emit process("ssh zuozhe@cs13.tcl-mobile.com");
}

/**
 * @brief onReciveOutput
 */
void onReciveOutput(QString output){
    cout << output;
}

/**
 * @brief onReciveError
 */
void onReciveError(QString error){
    cout << error;
}
void onReciveInfo(QString info){
    cout << info;
}

//void onReciveStatus(ProcessState state){
//    cout << state;
//}

