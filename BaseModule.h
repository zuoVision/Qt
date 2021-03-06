#ifndef BaseModule_H
#define BaseModule_H

#include <QObject>

#include "ProcessorImpl.h"


class BaseModule : public QObject
{
    Q_OBJECT
public:
    explicit BaseModule(QObject *parent = nullptr);
    ~BaseModule();

public:
    typedef  QProcess::ProcessState ProcessState;

private:
    void init();
    void unInit();
    void init_connect();

public:
    void login();
    void logout();

public:
    QThread*        mBaseModuleThread;
    ProcessorImpl*  mBaseModuleProcessor;


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

#endif // BaseModule_H
