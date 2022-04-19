#ifndef BASEMODULE_H
#define BASEMODULE_H

#include <QObject>

class BaseModule : public QObject
{
    Q_OBJECT
public:
    explicit BaseModule(QObject *parent = nullptr);

signals:

};

#endif // BASEMODULE_H
