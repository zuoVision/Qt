#include "fileoperation.h"
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QList>
#include <QFile>

#define MY_TAG "FileOperation"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

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
    QFile file(filePath);
    cout << filePath;
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
    cout ;
    m_testResult->clear();
    cout <<"begin"<< m_testResult->size();
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
//            cout << m_modulename << m_totalTests << m_pass;
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
                            QString resulotion;
                            if(result == "fail"){
                                if(name.contains("[0]")||name.contains("[1]")){
                                    if (!m_resulotion->filter(name.left(name.size()-3)).isEmpty()){
                                        resulotion = m_resulotion->filter(name.left(name.size()-3)).first().split(",")[1];
                                    }
                                }else{
                                    if(!m_resulotion->filter(name).isEmpty()){
                                        resulotion = m_resulotion->filter(name).first().split(",")[1];
                                    }

                                }
                            }
//                            cout << name << result << resulotion;
                            m_testResult->push_back(QStringList()<<testcase+"#"+name << result << resulotion);
                        }
                    }
                }
            }
        }
    }
    cout <<"end"<< m_testResult;
    if(m_modulename.isEmpty())
    {
        return false;
    }

    return true;
}

bool FileOperation::readCsv(QFile *file)
{
//    cout << file;
    QTextStream in(file);
    QString line;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.isNull()){
            file->close();
            break;
        }
        if(!line.isEmpty()){
            *m_resulotion << line;
        }
    }
//    cout << *m_resulotion;
    return true;
}
