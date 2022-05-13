#ifndef PROCESSORIMPL_H
#define PROCESSORIMPL_H

#include <QObject>
#include <QProcess>

struct Property{
    int                     id;
    QString                 name;
    QProcess::ProcessState  status;
};

class ProcessorImpl : public QObject
{
    Q_OBJECT
public:
    explicit ProcessorImpl(QObject *parent = nullptr);
    ~ProcessorImpl();

public:
    typedef  QProcess::ProcessState ProcessState;

private:
    void init();
    void uninit();
    void initConnect(QProcess* processor);

public:
    Property                mProp;
    QProcess*               mProcessor;
    QString                 mUserName;
    ProcessState            mState;
    QString                 mOutput;
    QString                 mError;


private:
    void getUserName(QString userName);


signals:
    void onSubmitOutput(QString);
    void onSubmitError(QString);
    void onSubmitInfo(QString);
    void onSubmitStatus(ProcessState);

private slots:
    void create();
    void start();
    void process(QString cmd);
    void stop();
    void kill();
    void exit();

    void onOutputListener();
    void onErrorListener();
    void onStatusListener(QProcess::ProcessState state);

};

#endif // PROCESSORIMPL_H
