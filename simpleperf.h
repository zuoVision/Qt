#ifndef SIMPLEPERF_H
#define SIMPLEPERF_H

#include <QKeyEvent>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTime>
#include "listenerthread.h"

class Simpleperf: public QObject
{
    Q_OBJECT
public:
    Simpleperf(QObject *parent =nullptr);

    //后台监听进程
    ListenerThread * listener;

    void init_connect();
    void wait();
    void processKeyPressEvent(QKeyEvent *event);
    void runCmdLine(QString cmd);
    void runAdbDevices(QString cmd);
    void runAdbRoot(QString cmd);
    void runAdbRemount(QString cmd);
    void runAdbOemUnlock(QString cmd);
    void runSimpleperfStat(QString cmd);
    void runSimpleperfRecord(QString cmd);
    void runSimpleperfReport(QString cmd);
    void runflamegraph();



private:

//    QThread     *listenerThread;
    QStringList m_cmd;
    QString     m_msg;
    bool        m_isProcessFinished = false;
    QTime       time;
protected:

signals:
    void signalToMainWindow(QString msg);
    void signalToMainWindow(QProcess::ProcessState state);
    void signalToListenerThread(QStringList msg);
    void signalToListenerInitThread();

private slots:
    void slotReciveListener(QString msg);
    void slotReciveProcessState(QProcess::ProcessState newState);
    void slotProcessfinished();
};

#endif // SIMPLEPERF_H
