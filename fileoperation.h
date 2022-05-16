#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <QFile>
#include <QObject>
#include <QXmlStreamReader>

class FileOperation : public QObject
{
    Q_OBJECT
public:
    explicit FileOperation(QObject *parent = nullptr);
    void loadDataBase(QString filePath,QStringList *stringList);
    void saveDataBase(QString filePath,QStringList *stringList);
    bool readXml(QFile *file);
    bool readCsv(QFile *file);
    QStringList readTxt(QFile *file);

public:
    QString                 m_modulename;
    QString                 m_totalTests;
    QString                 m_pass;
    QStringList             *m_resulotion=new QStringList();
    QVector<QStringList>    *m_testResult=new QVector<QStringList>();

private:
    QXmlStreamReader        xmlreader;
signals:

};

#endif // FILEOPERATION_H
