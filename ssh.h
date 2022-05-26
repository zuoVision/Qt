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

    typedef QVector<PROJECT>        ProjectInfo;


private:
    void init();
    void unInit();
    void init_connect();

public:
    void login(QString addr,METADATA* metadata);
    void logout();
    void run(QString cmd);
    void run(QString cmd,METADATA* metadata);
    void terminal();


public:
    QThread*        mSshThread;
    ProcessorImpl*  mSshProcessor;

private:
    COLOR           color;

signals:
    void create();
    void start();
    void process(QString);
    void process(QString,METADATA*);
    void stop();
    void kill();
    void exit();

signals:
    void onSubmitOutput(QString);
    void onSubmitError(QString);
    void onSubmitInfo(QString);
    void onSubmitState(int,QProcess::ProcessState);
    void onSubmitExitStatus(int,QProcess::ExitStatus);
    void onSubmitMetadata(METADATA*);

private slots:
    void onReciveOutput(QString output);
    void onReciveError(QString error);
    void onReciveInfo(QString info);
    void onReciveState(QProcess::ProcessState state);
    void onReciveExitStatus(QProcess::ExitStatus exitStatus);
    void onReciveMetadata(METADATA* metadata);
};

#endif // SSH_H
