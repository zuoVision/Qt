#include "xts.h"

#include <QDebug>

#define MY_TAG "XTS"


Xts::Xts(QObject *parent) : QObject(parent)
{

}

void Xts::runCts()
{
    qDebug() << MY_TAG << "runCts";
    listenerThread->openTerminal();
}
