#ifndef XTS_H
#define XTS_H

#include <QObject>

#include "listenerthread.h"

class Xts : public QObject
{
    Q_OBJECT
public:
    explicit Xts(QObject *parent = nullptr);
public:
    void runCts();

public:
    ListenerThread *listenerThread = new ListenerThread();
signals:

};

#endif // XTS_H
