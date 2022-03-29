#ifndef SIMPLEPERF_H
#define SIMPLEPERF_H

#include <QKeyEvent>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTime>

#include "commandprocessthread.h"

class Simpleperf: public QObject
{
    Q_OBJECT
public:
    Simpleperf(QObject *parent =nullptr);
    ~Simpleperf();

public:
    QThread                 *sim_Thread;
    CommandProcessThread    *sim_cpt;

    void init_connect();
    void processKeyPressEvent(QKeyEvent *event);

    void runStat();
    void runRecord();
    void runReport();
    void runFlamegraph();
    void stopProcessor();

private:
    QProcess::ProcessState m_state = QProcess::ProcessState::NotRunning;

protected:

signals:
    void start();
    void processCommand(QString cmd);
    void stop();
    void sig_sendToMainWindow(QString);
    void sig_sendToMainWindow(QProcess::ProcessState,QString);

private slots:
    void slo_reciveOutput(QString output);
    void slo_reciveError(QString error);
    void slo_reciveInfo(QString info);
    void slo_reciveState(QProcess::ProcessState state);

};

#endif // SIMPLEPERF_H
