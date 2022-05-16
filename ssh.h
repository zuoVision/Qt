#ifndef SSH_H
#define SSH_H

#include <QObject>

#include "ProcessorImpl.h"
#include "utils/general/general.h"

class Ssh : public QObject
{
    Q_OBJECT
public:
    explicit Ssh(QObject *parent = nullptr);
    ~Ssh();

public:
    typedef QProcess::ProcessState  ProcessState;
    typedef QVector<PROJECT>        ProjectInfo;

private:
    void init();
    void unInit();
    void init_connect();

public:
    void login(QString addr);
    void logout();
    void run(QString cmd);


public:
    QThread*        mSshThread;
    ProcessorImpl*  mSshProcessor;

private:
    COLOR           color;

signals:
    void create();
    void start();
    void process(QString cmd);
    void stop();
    void kill();
    void exit();

signals:
    void onSubmitOutput(QString);
    void onSubmitError(QString);
    void onSubmitInfo(QString);
    void onSubmitState(int,QProcess::ProcessState);
    void onSubmitExitStatus(int,QProcess::ExitStatus);

private slots:
    void onReciveOutput(QString output);
    void onReciveError(QString error);
    void onReciveInfo(QString info);
    void onReciveState(QProcess::ProcessState state);
    void onReciveExitStatus(QProcess::ExitStatus exitStatus);

};

#endif // SSH_H
