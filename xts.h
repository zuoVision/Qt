#ifndef XTS_H
#define XTS_H

#include <QBuffer>
#include <QObject>

#include "listenerthread.h"
#include "commandprocessthread.h"
#include "ProcessorImpl.h"


class Xts : public QObject
{
    Q_OBJECT
public:
    explicit Xts(QObject *parent = nullptr);
    ~Xts();

public:
    void init();
    void unInit();
    void init_connect();



public:
    QThread                 *mXtsThread;
    ProcessorImpl           *mXtsProcessor;
    bool                    m_ctsComplete=false;
    bool                    m_flag=false;
    QString                 m_ctsSuite;
    QString                 m_ctsCommand;
    QString                 m_totalRunTime;
    QString                 m_totalTests;
    QString                 m_passed;
    QString                 m_failed;

    COLOR                   color;

public:
    void run(const QString arg1,const QString arg2,QString arg3,QString arg4);
    void run(const QString cmd,METADATA* matadata);
    void terminal();

private:
    void analyzeResult(QString output);

//signals:
//    void start();
//    void processCommand(QString);
//    void processCommand(QString,QString *);
//    void stop();
//    void exit();
//    void sig_sendToMainWindow(QString);
//    void sig_sendToMainWindow(QProcess::ProcessState,QString);
//    void sig_findCtsSuite();
//    void sig_showCtsResult();

//public slots:
//    void slo_reciveOutput(QString output);
//    void slo_reciveError(QString error);
//    void slo_reciveInfo(QString info);
//    void slo_reciveState(QProcess::ProcessState state);
//    void slo_reciveMainWindow(QString msg);

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
    void onSubmitMetadata(METADATA *metadata);
    void onShowCtsResult();

private slots:
    void onReciveOutput(QString output);
    void onReciveError(QString error);
    void onReciveInfo(QString info);
    void onReciveState(QProcess::ProcessState state);
    void onReciveExitStatus(QProcess::ExitStatus exitStatus);
    void onReciveMetadata(METADATA *metadata);

};

#endif // XTS_H
