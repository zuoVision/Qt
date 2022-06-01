#ifndef SIMPLEPERF_H
#define SIMPLEPERF_H

#include <QKeyEvent>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTime>

#include "utils/general/general.h"
#include "commandprocessthread.h"
#include "ProcessorImpl.h"

class Simpleperf: public QObject
{
    Q_OBJECT
public:
    Simpleperf(QObject *parent =nullptr);
    ~Simpleperf();

public:
    QThread                 *mSimThread;
    ProcessorImpl           *mSimProcessor;

    void init();
    void unInit();
    void init_connect();
    void processKeyPressEvent(QKeyEvent *event);

    void runList();
    void runStat();
    void runStat(std::map<QString,QString>* statParams);
    void runRecord();
    void runRecord(std::map<QString,QString>* recordParams);
    void runReport();
    void runFlamegraph();
//    void runQuickGeneration();

public:
    void run(QString cmd);
    void terminal();

private:
    ProcessState m_state = ProcessState::NotRunning;
    COLOR  color;

protected:


signals:
    void create();
    void start();
    void process(QString);
    void stop();
    void kill();
    void exit();

/*new*/
signals:
    void onSubmitOutput(QString);
    void onSubmitError(QString);
    void onSubmitInfo(QString);
    void onSubmitState(int,QProcess::ProcessState);
    void onSubmitExitStatus(int,QProcess::ExitStatus);

/*new*/
private slots:
    void onReciveOutput(QString output);
    void onReciveError(QString error);
    void onReciveInfo(QString info);
    void onReciveState(QProcess::ProcessState state);
    void onReciveExitStatus(QProcess::ExitStatus exitStatus);

};

#endif // SIMPLEPERF_H
