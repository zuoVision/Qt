#include "fileoperation.h"
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QList>

#define MY_TAG "FileOperation"

FileOperation::FileOperation(QObject *parent) : QObject(parent)
{
    qDebug()<<MY_TAG<<QThread::currentThreadId();
}

void FileOperation::loadDataBase(QString filePath, QStringList *stringList)
{
    QFile file(filePath);
    qDebug()<<MY_TAG<<"loadDataBase"<<file.fileName()<<stringList;

    if(!file.exists()){
        QMessageBox::warning(NULL,"warning",QString("file:%1 does not exist!")
                             .arg(file.fileName()));

    }
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(NULL,"warning",QString("can't open file:%1(%2)")
                             .arg(file.fileName())
                             .arg(file.errorString()));
    }
    QTextStream in(&file);
    QString line;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.isNull()){
            file.close();
            break;
        }
        if (!line.isEmpty()&&!stringList->contains(line))
            stringList->append(line);
    }
    //    return stringList;
}

void FileOperation::saveDataBase(QString filePath, QStringList *stringList)
{
    qDebug()<<MY_TAG<<"saveDataBase"<<filePath<< *stringList;
    QFile file(filePath);
    if(!file.exists()){
        QMessageBox::warning(NULL,"warning",QString("file:%1 does not exist!")
                             .arg(file.fileName()));

    }
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(NULL,"warning",QString("can't open file:%1(%2)")
                             .arg(file.fileName())
                             .arg(file.errorString()));
    }
    QTextStream out(&file);

    for(QList<QString>::iterator it=stringList->begin();it!=stringList->end();it++){
        out<<*it<<"\n";
    }
    file.close();
}
