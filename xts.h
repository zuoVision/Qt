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
    void runCts();

public:
//    ListenerThread *listenerThread = new ListenerThread();
    CommandProcessThread * cpt;
signals:
    void signalToListenerThread(QStringList cmd);

};

#endif // XTS_H
