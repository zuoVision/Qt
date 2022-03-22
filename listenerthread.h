#ifndef LISTENERTHEARED_H
#define LISTENERTHEARED_H

#include <QProcess>
#include <QThread>
#include <QObject>
#include <QVector>

#define BASH    "bash"
//#define ADB     "adb shell"

class ListenerThread : public QThread
{
    Q_OBJECT
public:
    ListenerThread(/*QObject *parent =nullptr*/);
    enum SignalType {
        NORNAL_INFO=0,
        OUTPUT_INFO,
        ERROR_INFO
    };
    Q_ENUM(SignalType)
    // QThread interface
    QString     m_userName;
    QString     m_hostName;
    QStringList m_cmd;
    QString     m_stdOutput;
    QString     m_stdError;
    QString     m_program = BASH;

    QProcess *p;

    bool isExit(QStringList info);
    void run();
private:
    void initThread();
    void process();




protected:

signals:
    void signalToMainWindow(QString info);
//    void signalToSimpleperf(QString info);
    void signalToSimpleperf(QString info,ListenerThread::SignalType signalType=NORNAL_INFO);
    void signalToSimpleperf(QProcess::ProcessState newState);
    void signalProcessFinished();

private slots:
    void slotReciveMainWindow(QStringList info);
    void slotReciveSimpleperf(QStringList info);
    void slotReciveProcessState(QProcess::ProcessState newState);
};

#endif // LISTENERTHEARED_H
