#ifndef PARSEXML_H
#define PARSEXML_H

#include <QFile>
#include <QObject>

#include <utils/general/general.h>


typedef QVector<PROJECT> ProjectInfo;

class ParseXml : public QObject{
public:

    ParseXml(QObject *parent=nullptr);
    ~ParseXml();

private:


public:

    ProjectInfo parseProjectXml(QString filePath);
    void readXml(QFile* file,ProjectInfo* projectInfo);
    QStringList searchBranch(const QString projectName,ProjectInfo* project);
    QString searchRepo(const QString projectName,const QString branchName,ProjectInfo* project);
    QStringList searchBuildVer(const QString projectName,ProjectInfo* project);
    QString searchBuildCmd(const QString projectName,const QString buildVer,ProjectInfo* project);

};


#endif // PARSEXML_H
