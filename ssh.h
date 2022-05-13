#ifndef SSH_H
#define SSH_H

#include <QObject>

#include "ProcessorImpl.h"


class Ssh : public QObject
{
    Q_OBJECT
public:
    explicit Ssh(QObject *parent = nullptr);
    ~Ssh();

public:
    typedef  QProcess::ProcessState ProcessState;

private:
    void init();
    void unInit();
    void init_connect();

public:
    void login();

public:
    QThread*        mSshThread;
    ProcessorImpl*  mSshProcessor;


signals:
    void create();
    void start();
    void process(QString cmd);
    void stop();
    void kill();
    void exit();

private slots:
//    void onReciveOutput(QString output);
//    void onReciveError(QString error);
//    void onReciveInfo(QString info);
//    void onReciveStatus(ProcessState state);

};

#endif // SSH_H
