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
    void initConnect();

public:
    Property                mProp;
    QProcess*               mProcessor;
    QString                 mUserName;
    ProcessState            mState;
    QString                 mOutput;
    QString                 mError;


private:
    void            setUserName(QString userName);
    void            setState(QProcess::ProcessState state);

public:
    QString         getUserName();
    ProcessState    getState();

signals:
    void onSubmitOutput(QString);
    void onSubmitError(QString);
    void onSubmitInfo(QString);
    void onSubmitState(QProcess::ProcessState);
    void onSubmitExitStatus(QProcess::ExitStatus);

private slots:
    void create();
    void start();
    void process(QString cmd);
    void stop();
    void kill();
    void exit();


    void onOutputListener();
    void onErrorListener();
    void onStateListener(QProcess::ProcessState state);
    void onFinishedListener(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // PROCESSORIMPL_H
