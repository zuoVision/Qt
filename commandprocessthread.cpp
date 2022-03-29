#include "commandprocessthread.h"

#include <QDebug>


#define MY_TAG "CommandProcessThread"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]"

#define BASH "bash"

CommandProcessThread::CommandProcessThread(QObject *parent):QObject(parent)
{
    cout <<QThread::currentThreadId();
//    init();
}

CommandProcessThread::~CommandProcessThread()
{
    cout << "[~CommandProcessThread]";

    uninit();
}

void CommandProcessThread::init()
{
    cout <<QThread::currentThreadId();

}

void CommandProcessThread::uninit()
{
    cout <<QThread::currentThreadId();
//    processor->terminate();
//    delete processor;
}

void CommandProcessThread::init_connect()
{
    connect(processor,SIGNAL(readyReadStandardOutput()),
            this,SLOT(slo_processAllMsg()));
    connect(processor,SIGNAL(readyReadStandardError()),
            this,SLOT(slo_processAllMsg()));
    qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
    connect(processor,SIGNAL(stateChanged(QProcess::ProcessState)),
            this,SLOT(slo_processState(QProcess::ProcessState)));
}

void CommandProcessThread::createProcessor()
{
    cout << QThread::currentThreadId();
    processor = new QProcess();
    processor->start(BASH,QStringList() << "-c" << "whoami;hostname"); //hostname:41000966-26-1 username:zuozhe
    processor->waitForFinished();
    //zuozhe@41000966-26-1:~$
    m_userName = processor->readAllStandardOutput();
    QStringList name = m_userName.split("\n");
    if(!name.isEmpty()) name.pop_back();
    m_userName = name.first().append("@").append(name.last()).append(":~$ ");
    qDebug() << MY_TAG << QThread::currentThreadId() << m_userName;
    init_connect();
}

void CommandProcessThread::stopProcessor()
{
    cout << QThread::currentThreadId();
    if(processor->state()!=QProcess::ProcessState::NotRunning)
    {
        processor->close();
        return emit sig_sendInfo("Force Exit!(Ctrl+D)");
    }
}

void CommandProcessThread::exitProcessor()
{
    cout << "[exitProcessor]";
    delete processor;
}

void CommandProcessThread::process(QString cmd)
{
    cout << " + ";
    if (cmd.isEmpty())
    {
        cout << "command is empty, please input command!";
        return;
    }
    if (processor->state()!=QProcess::ProcessState::NotRunning){
        if (cmd.contains("exit")){
            processor->close();
            return emit sig_sendInfo("Force Exit!");
        }
        return emit sig_sendInfo("please wait!");
    }
    processor->start(BASH,QStringList()<<"-c"<< cmd);
    processor->waitForReadyRead();
    qDebug() << MY_TAG << "[process]"<<" - ";
}

void CommandProcessThread::process(QString cmd, QString *callback)
{
    cout << " + " <<"callback"<< *callback;
    if (cmd.isEmpty())
    {
        cout << "command is empty, please input command!";
        return;
    }
    if (processor->state()!=QProcess::ProcessState::NotRunning){
        if (cmd.contains("exit")){
            processor->close();
            return emit sig_sendInfo("Force Exit!");
        }
        return emit sig_sendInfo("please wait!");
    }
    processor->start(BASH,QStringList()<<"-c"<< cmd);
    if(!processor->waitForFinished()){
        cout << "time out";
        processor->close();
    }
    cout <<"callback"<< callback << *callback;
    cout << " - ";
}

void CommandProcessThread::slo_processAllMsg()
{
    m_output = processor->readAllStandardOutput();
    m_error = processor->readAllStandardError();
    //remove endwith "\n"
    m_output.replace(QRegExp("\n$"), "");
    m_error.replace(QRegExp("\n$"), "");
    if(!m_output.isEmpty()) emit sig_sendOutput(m_output);
    if(!m_error.isEmpty())  emit sig_sendOutput(m_error);
}

void CommandProcessThread::slo_processState(QProcess::ProcessState state)
{
//    qDebug() << MY_TAG << "[slo_reciveState]" << state;
    emit sig_sendState(state);
}




