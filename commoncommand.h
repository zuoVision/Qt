#ifndef COMMONCOMMAND_H
#define COMMONCOMMAND_H

#include <QObject>

#include "basemodule.h"
#include "commandprocessthread.h"

class CommonCommand : public QObject
{
    Q_OBJECT
public:
    explicit CommonCommand(QObject *parent = nullptr);
    ~CommonCommand();

    void init();
    void uninit();
    void init_connect();
    void onCreateThread(QThread * prtThead,CommandProcessThread * ptrCPT);
    void runCommand(const QString cmd);
    void stopProcessor();

public:
    QThread *ccd_Thread;
    CommandProcessThread *ccd_cpt;

signals:
    void start();
    void processCommand(QString);
    void stop();
    void sig_sendToMainWindow(QString);
    void sig_sendToMainWindow(QProcess::ProcessState,QString);

public slots:
    void slo_reciveOutput(QString output);
    void slo_reciveError(QString error);
    void slo_reciveInfo(QString info);
    void slo_reciveState(QProcess::ProcessState state);
};

#endif // COMMONCOMMAND_H
