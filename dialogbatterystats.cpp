#include "dialogbatterystats.h"
#include "ui_dialogbatterystats.h"
#include "cmd.h"
#include "document.h"

#include <QDebug>
#define MY_TAG "DialogBatterystats"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"


DialogBatterystats::DialogBatterystats(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBatterystats)
{
    ui->setupUi(this);
    init();
}

DialogBatterystats::~DialogBatterystats()
{
    delete ui;
}

void DialogBatterystats::init()
{
    ui->label_batteryhistorianurl->setText(BATTERYHISTORIANURL);
}

void DialogBatterystats::initConnect()
{

}

void DialogBatterystats::on_pushButton_reset_clicked()
{
    emit sig_batterystat(BATTERYSTATS " --reset");
}

void DialogBatterystats::on_pushButton_open_clicked()
{
    Document doc;
    QString path = doc.selectDirectory("");
    ui->lineEdit_savedatapath->setText(path);
}


void DialogBatterystats::on_pushButton_open_2_clicked()
{
    QString cmd = BATTERYSTATS;
    if(ui->lineEdit_savedatapath->text().isEmpty()){
        cmd += QString("%1 %2%3")
                .arg(" > batterystats.txt;")
                .arg(BUGREPORT)
                .arg(" bugreport.zip");
    }else{
        cmd += QString("%1 %2%3 %4 %5%6")
                .arg(" > ")
                .arg(ui->lineEdit_savedatapath->text())
                .arg("/batterystats.txt;")
                .arg(BUGREPORT)
                .arg(ui->lineEdit_savedatapath->text())
                .arg("/bugreport.zip");
    }

    cout << cmd;
    emit sig_batterystat(cmd);
}
