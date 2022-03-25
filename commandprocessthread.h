#ifndef COMMANDPROCESSTHREAD_H
#define COMMANDPROCESSTHREAD_H

#include <QProcess>
#include <QThread>
#include <QVector>

class CommandProcessThread : public QThread
{
    Q_OBJECT
public:
    CommandProcessThread();
    void createProcessor();

private:
    void run() override;
    void process();

public:
    QProcess *p1;
    QProcess *p2;
    QVector<QProcess*> processors;
private:



signals:

public slots:
    void printOutput();
};

#endif // COMMANDPROCESSTHREAD_H
