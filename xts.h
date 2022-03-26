#ifndef XTS_H
#define XTS_H

#include <QObject>

#include "listenerthread.h"
#include "commandprocessthread.h"


class Xts : public QObject
{
    Q_OBJECT
public:
    explicit Xts(QObject *parent = nullptr);
    ~Xts();

public:
    void init_connect();
    void findCtsSuite();
    void runCts(const QString arg1,const QString arg2,const QString arg3,const QString arg4);
    void stopProcessor();

public:
    QThread                 *xts_Thread;
    CommandProcessThread    *xts_cpt;

    QStringList             m_ctsSuite;

signals:
    void start();
    void processCommand(QString cmd);
    void processCommand(QString cmd,unsigned long secs);
    void stop();
    void exit();
    void sig_sendToMainWindow(QString);
    void sig_sendToMainWindow(QProcess::ProcessState);
    void sig_findCtsSuite();

public slots:
    void slo_reciveOutput(QString output);
    void slo_reciveError(QString error);
    void slo_reciveInfo(QString info);
    void slo_reciveState(QProcess::ProcessState state);
};

#endif // XTS_H
