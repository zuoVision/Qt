#ifndef PROCESSORIMPL_H
#define PROCESSORIMPL_H

#include <QObject>
#include <QProcess>

#include "utils/general/general.h"

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
    typedef QProcess::ProcessState ProcessState;
    typedef std::function<void ()> CallbackFun;

private:
    void init();
    void uninit();
    void initConnect();

public:
    Property                mProp;
    QProcess*               mProcessor;
    QString                 mUserName;
    ProcessState            mState;
    QString                 mOutput;
    QString                 mError;
    METADATA*               mMetadata=nullptr;

private:
    void            setUserName(QString userName);
    void            setState(QProcess::ProcessState state);

public:
    QString         getUserName();
    ProcessState    getState();

private:


signals:
    void onSubmitOutput(QString);
    void onSubmitError(QString);
    void onSubmitInfo(QString);
    void onSubmitState(QProcess::ProcessState);
    void onSubmitExitStatus(QProcess::ExitStatus);
    void onSubmitMetadata(METADATA*);

private slots:
    void create();
    void start();
    void process(QString cmd);
    void process(QString cmd,METADATA* metadata);
    void stop();
    void kill();
    void exit();


    void onOutputListener();
    void onErrorListener();
    void onStateListener(QProcess::ProcessState state);
    void onFinishedListener(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // PROCESSORIMPL_H
