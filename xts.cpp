#include "xts.h"

#include <QDebug>

#define MY_TAG "XTS"
#define CTSSUITE "~/XTS/Android_R/android-cts/tools/cts-tradefed "
#define CTRCOMMAND "run cts-dev "
#define CTSMODULE "-m CtsCameraTestCases "
#define CTSTEST "-t android.hardware.camera2.cts.RobustnessTest#testSimultaneousTriggers"

Xts::Xts(QObject *parent) : QObject(parent)
{
    cpt = new CommandProcessThread();
    cpt->start();
}

Xts::~Xts()
{
    qDebug()<<MY_TAG<<"~Xts()";
    cpt->quit();
}

void Xts::runCts()
{
    qDebug() << MY_TAG << "runCts";
//    QStringList cts;
//    QString cmd = QString(CTSSUITE).append(CTRCOMMAND).append(CTSMODULE).append(CTSTEST);
//    cts << "-c" << cmd;
//    emit signalToListenerThread(cts);
}
