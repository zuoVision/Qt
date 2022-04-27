#ifndef COMMANDPROCESSTHREAD_H
#define COMMANDPROCESSTHREAD_H

#include <QProcess>
#include <QThread>

class CommandProcessThread : public QObject
{
    Q_OBJECT
public:
    CommandProcessThread(QObject *parent=nullptr);
    CommandProcessThread(QString threadName);
    ~CommandProcessThread();

private:
    void init();
    void uninit();
    void init_connect();


public:
    QString                 m_ThreadName=NULL;
    QProcess                *processor;
    QProcess::ProcessState  m_processState;
    QString                 m_userName;
//    QString                 m_hostName;
    QString                 m_output;
    QString                 m_error;


private:
    bool m_flag=false;

signals:
    void sig_sendOutput(QString ouput);
    void sig_sendError(QString error);
    void sig_sendState(QProcess::ProcessState state);
    void sig_sendInfo(QString info);

public slots:
    void process(QString cmd);
    void process(QString cmd,QString *callback);
    void stopProcessor();
    void exitProcessor();
    void createProcessor();
    void slo_processAllMsg();
    void slo_processState(QProcess::ProcessState state);
};

#endif // COMMANDPROCESSTHREAD_H
