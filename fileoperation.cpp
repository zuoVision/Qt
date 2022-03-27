#include "fileoperation.h"
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QList>
#include <QFile>

#define MY_TAG "FileOperation"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]"

FileOperation::FileOperation(QObject *parent) :QObject(parent)
{
    cout <<QThread::currentThreadId();
}

void FileOperation::loadDataBase(QString filePath, QStringList *stringList)
{
    QFile file(filePath);
    cout<<file.fileName()/*<<stringList*/;

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
//    cout <<filePath.split("/").last()<< *stringList;
    QFile file(filePath.split("/").last());
    if(!file.exists()){
        QMessageBox::warning(NULL,"warning",QString("file:%1 does not exist!")
                             .arg(file.fileName()));
    }
    if(!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text)){   //QIODevice::Text 换行符生效
        QMessageBox::warning(NULL,"warning",QString("can't open file:%1(%2)")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        file.close();
        return;
    }
    QTextStream out(&file);

    for(QList<QString>::iterator it=stringList->begin();it!=stringList->end();it++){
        out<<*it<<"\n";
    }
    file.close();
    return;
}

bool FileOperation::readXml(QFile *file)
{
    QString nodename;
    QString output;
    QXmlStreamReader xmlreader(file);
    while (!xmlreader.atEnd() || !xmlreader.hasError()) {
        xmlreader.readNextStartElement();
        nodename = xmlreader.name().toString();
        if(nodename == "Module" && xmlreader.isStartElement()){
            m_modulename = xmlreader.attributes().value("name").toString();
            m_totalTests = xmlreader.attributes().value("total_tests").toString();
            m_pass       = xmlreader.attributes().value("pass").toString();
            while (!(nodename == "Module" && xmlreader.isEndElement())) {// module not end
                xmlreader.readNextStartElement();
                nodename = xmlreader.name().toString();
                if(nodename == "TestCase" && xmlreader.isStartElement()){
                    QString testcase = xmlreader.attributes().value("name").toString();
                    while (!(nodename == "TestCase" && xmlreader.isEndElement())) {
                        xmlreader.readNextStartElement();
                        nodename = xmlreader.name().toString();
                        if(nodename == "Test" && xmlreader.isStartElement()){
                            QString name    = xmlreader.attributes().value("name").toString();
                            QString result  = xmlreader.attributes().value("result").toString();
                            m_test   << testcase+"#"+name;
                            m_result << result;
                        }
                    }
                }
            }
        }
    }
    if(m_modulename.isEmpty()||xmlreader.hasError()) return false;
    return true;
}
