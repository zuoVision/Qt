#ifndef BASEMODULE_H
#define BASEMODULE_H

#include "commandprocessthread.h"

#include <QObject>
#include <QProcess>

class BaseModule : public QObject
{
    Q_OBJECT
public:
    explicit BaseModule(QObject *parent = nullptr);
    ~BaseModule();

public:
    QThread                 *mThread;
    CommandProcessThread    *mCpt;


    void excute(CommandProcessThread * ptrCPT,const QString cmd);
    QString getHost();

protected:
    void init();
    void uninit();
    void init_connect(CommandProcessThread * ptrCPT);
    void onCreateThread(QThread * prtThead,CommandProcessThread * ptrCPT);
    void onDestroyThread(QThread * prtThead);
    void stopProcessor();

private:
    QProcess::ProcessState m_state = QProcess::ProcessState::NotRunning;

signals:
    void start();
    void process(QString);
    void stop();
    void exit();
    void sig_sendToMainWindow(QString);
    void sig_sendToMainWindow(QProcess::ProcessState,QString);
    void sig_sendToProcessThread(QString);

public slots:
    void slo_reciveOutput(QString output);
    void slo_reciveError(QString error);
    void slo_reciveInfo(QString info);
    void slo_reciveState(QProcess::ProcessState state);
    void slo_reciveMainWindow(QString msg);
    void slo_reciveProcessThread(QString msg);

};

#endif // BASEMODULE_H
