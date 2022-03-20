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
}

void Document::init()
{
    filePath = "README.md";
}

void Document::onLoadDocument(QString doc)
{
    QString displayString;
    QFile file(doc);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
//        qDebug()<<"Can't open the file! return"<<endl;
        return;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str(line);
//        qDebug() << str;
        displayString.append(str);
    }
    ui->plainTextEdit_doc->clear();
    ui->plainTextEdit_doc->appendPlainText(displayString);
}

void Document::on_pushButton_open_clicked()
{
    QFileDialog *fd = new QFileDialog(this);
    fd->setWindowTitle("选择文件*.txt");
    fd->setNameFilter("");
    fd->setViewMode(QFileDialog::Detail);
    QString filename;
    if(fd->exec() == QDialog::Accepted){
        filename = fd->selectedFiles()[0];
    }

    onLoadDocument(filename);

}

void Document::on_pushButton_save_clicked()
{

    QFile myfile(filePath);//创建一个输出文件的文档
    if (myfile.open(QFile::WriteOnly|QFile::Text))//注意WriteOnly是往文本中写入的时候用，ReadOnly是在读文本中内容的时候用，Truncate表示将原来文件中的内容清空
    {
        QTextStream out(&myfile);
        out << ui->plainTextEdit_doc->toPlainText();
    }
    QMessageBox::information(this,"提示","文本已保存");
}