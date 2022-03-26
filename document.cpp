#include "document.h"
#include "ui_document.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);
    init();
}

Document::~Document()
{
    delete ui;
    emit destroyed();
}

void Document::init()
{
    m_filePath = "README.md";
}

void Document::onLoadDocument(QString doc)
{
    QString displayString;
    qDebug() << doc;
    qDebug() << doc.split("/").last();
    QFile file(doc.split("/").last());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file! return"<<endl;
        return;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str(line);
        displayString.append(str);
    }
    ui->plainTextEdit_doc->clear();
    ui->plainTextEdit_doc->appendPlainText(displayString);
    file.close();
}

QString Document::openFile()
{
    QFileDialog *fd = new QFileDialog(this);
    QString filePath;
    fd->setWindowTitle("选择文件");
    fd->setNameFilter("");
    fd->setViewMode(QFileDialog::Detail);
    if(fd->exec() == QDialog::Accepted){
        filePath = fd->selectedFiles()[0];
    }
    return filePath;
}

void Document::closeEvent(QCloseEvent *)
{
    emit closed();
}

void Document::on_pushButton_open_clicked()
{
    m_filePath = openFile();
    onLoadDocument(m_filePath);
}

void Document::on_pushButton_save_clicked()
{
//    qDebug()<<filePath;
    QFile myfile(m_filePath);//创建一个输出文件的文档
    if (myfile.open(QFile::WriteOnly|QFile::Text))//注意WriteOnly是往文本中写入的时候用，ReadOnly是在读文本中内容的时候用，Truncate表示将原来文件中的内容清空
    {
        QTextStream out(&myfile);
        out << ui->plainTextEdit_doc->toPlainText();
    }
    QMessageBox::information(this,"提示","文本已保存");
    myfile.close();
}
