#ifndef COMMONCOMMAND_H
#define COMMONCOMMAND_H

#include <QObject>

#include "commandprocessthread.h"
#include "ProcessorImpl.h"
#include "utils/general/general.h"

class CommonCommand : public QObject
{
    Q_OBJECT
public:
    explicit CommonCommand(QObject *parent = nullptr);
    ~CommonCommand();

    void init();
    void unInit();
    void init_connect();
//    void onCreateThread(QThread * prtThead,CommandProcessThread * ptrCPT);
//    void runCommand(const QString cmd);
//    void stopProcessor();

public:
    QThread         *mCcdThread;
    ProcessorImpl   *mCcdProcessor;
    COLOR           color;

public:
    void run(QString cmd);
    void terminal();
//signals:
//    void start();
//    void processCommand(QString);
//    void stop();
//    void sig_sendToMainWindow(QString);
//    void sig_sendToMainWindow(QProcess::ProcessState,QString);

//public slots:
//    void slo_reciveOutput(QString output);
//    void slo_reciveError(QString error);
//    void slo_reciveInfo(QString info);
//    void slo_reciveState(QProcess::ProcessState state);

signals:
    void create();
    void start();
    void process(QString);
    void process(QString,ptrFunc);
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

#endif // COMMONCOMMAND_H
