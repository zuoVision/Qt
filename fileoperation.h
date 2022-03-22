#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <QFile>
#include <QObject>

class FileOperation : public QObject
{
    Q_OBJECT
public:
    explicit FileOperation(QObject *parent = nullptr);
    void loadDataBase(QString filePath,QStringList *stringList);
    void saveDataBase(QString filePath,QStringList *stringList);

signals:

};

#endif // FILEOPERATION_H
