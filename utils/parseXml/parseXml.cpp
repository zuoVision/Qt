#include "utils/parseXml/parseXml.h"
#include <QObject>
#include <QVector>
#include <QMultiMap>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

#include "utils/general/general.h"

#define MY_TAG          "ParseXml"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

/**
 * @brief ParseXml::ParseXml
 */
ParseXml::ParseXml(QObject *parent): QObject(parent)
{
    qDebug();
}

/**
 * @brief ParseXml::~ParseXml
 */
ParseXml::~ParseXml()
{
    qDebug();
}

/**
 * @brief ParseXml::parseProjectXml
 * @param file
 */
ProjectInfo ParseXml::parseProjectXml(QString filePath)
{
    qDebug() << filePath;

    ProjectInfo projectInfo;

    QFile file(filePath);
    if(!file.exists()){
        qDebug() << "file not exist:" << filePath;
        return projectInfo;
    }else if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "file open failed :"<< filePath;
    }else{
        readXml(&file,&projectInfo);
    }
    return projectInfo;
}

/**
 * @brief ParseXml::perseXml
 * @param file
 */
void ParseXml::readXml(QFile* file,ProjectInfo* projectInfo)
{

    QString projectName;
    QString branchName;
    QString repoCmd;
    QString buildVersion;
    QString buildCmd;
    QString nodeName;
    QXmlStreamReader xmlReader(file);
    while(!xmlReader.atEnd() || !xmlReader.hasError()){
        xmlReader.readNextStartElement();
        nodeName = xmlReader.name().toString();
        if(nodeName == "Project" && xmlReader.isStartElement()){
            PROJECT project;
            projectName = xmlReader.attributes().value("name").toString();
            project.projectName = projectName;
//            qDebug() << "[project name]" << projectName;
            while(!(nodeName == "Project" && xmlReader.isEndElement())){
                xmlReader.readNextStartElement();
                nodeName = xmlReader.name().toString();
                if(nodeName == "branch" && xmlReader.isStartElement()){
                    branchName = xmlReader.attributes().value("name").toString();
//                    qDebug() << "[branch name]" << branchName;
                    while (!(nodeName == "branch" && xmlReader.isEndElement())) {
                        xmlReader.readNextStartElement();
                        nodeName = xmlReader.name().toString();
                        if(nodeName == "repo" && xmlReader.isStartElement()){
                            repoCmd = xmlReader.attributes().value("cmd").toString();
                            BRANCH branch;
                            branch.branchName = branchName;
                            branch.repo.repoCmd = repoCmd;
                            project.branch.push_back(branch);
//                            qDebug() << "[repo cmd]" << repoCmd;
                        }
                    }
                }
                if(nodeName == "build" && xmlReader.isStartElement()){
                    while (!(nodeName == "build" && xmlReader.isEndElement())) {
                        BUILD build;
                        xmlReader.readNextStartElement();
                        nodeName = xmlReader.name().toString();
                        if(nodeName == "version" && xmlReader.isStartElement()){
                            buildVersion = xmlReader.attributes().value("ver").toString();
                            buildCmd     = xmlReader.attributes().value("cmd").toString();
                            build.insert(buildVersion,buildCmd);
                            project.build.push_back(build);
                            qDebug() << "[build version1]" << buildVersion;
                            qDebug() << "[build version2]" << build.first();
                            qDebug() << "[build cmd1]" << buildCmd;
                            qDebug() << "[build cmd2]" << build.last();
                        }
                    }
                }
            }
            projectInfo->push_back(project);
        }
    }
}

/**
 * @brief ParseXml::searchBranch
 * @param keyword
 * @param project
 * @return branch list
 */
QStringList ParseXml::searchBranch(const QString projectName, ProjectInfo *project)
{
    QStringList res;
    for(QVector<PROJECT>::iterator itProject = project->begin();
        itProject!=project->end();itProject++){
        if(projectName == itProject->projectName){
            for(QVector<BRANCH>::iterator itBranch = itProject->branch.begin();
                itBranch != itProject->branch.end();itBranch++){
                res.append(itBranch->branchName);
            }
            return res;
        }
    }
    return res;
}

/**
 * @brief ParseXml::searchRepo
 * @param projectName
 * @param branchName
 * @param project
 * @return repo cmd
 */
QString ParseXml::searchRepo(const QString projectName, const QString branchName, ProjectInfo *project)
{
    QString res;
    for(QVector<PROJECT>::iterator itProject = project->begin();
        itProject!=project->end();itProject++){
        if(projectName == itProject->projectName){
            for(QVector<BRANCH>::iterator itBranch = itProject->branch.begin();
                itBranch != itProject->branch.end();itBranch++){
                if(branchName == itBranch->branchName){
                    res.append(itBranch->repo.repoCmd);
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief ParseXml::searchBuildVer
 * @param projectName
 * @param project
 * @return build versions
 */
QStringList ParseXml::searchBuildVer(const QString projectName, ProjectInfo *project)
{
    QStringList res;
    for(QVector<PROJECT>::iterator itProject = project->begin();
        itProject!=project->end();itProject++){
        if(projectName == itProject->projectName){
            for(QVector<BUILD>::iterator itBuild = itProject->build.begin();
                itBuild != itProject->build.end();itBuild++){
                res.append(itBuild->keys().first());
            }
            //add ninja
            res.append("ninja");
            return res;
        }
    }
    return res;
}

/**
 * @brief ParseXml::searchBuildCmd
 * @param projectName
 * @param buildVer
 * @param project
 * @return build cmd
 */
QString ParseXml::searchBuildCmd(const QString projectName, const QString buildVer, ProjectInfo *project)
{
    cout;
    QString res;
    for(QVector<PROJECT>::iterator itProject = project->begin();
        itProject!=project->end();itProject++){
        if(projectName == itProject->projectName){
            for(QVector<BUILD>::iterator itBuild = itProject->build.begin();
                itBuild != itProject->build.end();itBuild++){
                if(buildVer == itBuild->keys().first()){
                    res = itBuild->values().first();
                    return res;
                }
            }
        }
    }
    return res;
}


