#include "dialogbatterystats.h"
#include "ui_dialogbatterystats.h"
#include "cmd.h"
#include "document.h"

#include <QDebug>
#include <QMessageBox>
#define MY_TAG "DialogBatterystats"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"


DialogBatterystats::DialogBatterystats(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBatterystats)
{
    ui->setupUi(this);
    init();
    initConnect();
}

DialogBatterystats::~DialogBatterystats()
{
    delete ui;
}

void DialogBatterystats::init()
{
    ui->label_batteryhistorianurl->setText(BATTERYHISTORIANURL);
    ui->label_batteryhistorianurl->setOpenExternalLinks(true);

    //timer
    timer = new QTimer();
    currentStatus = STATUS::startStats;
}

void DialogBatterystats::initConnect()
{
    connect(timer,SIGNAL(timeout()),
            this,SLOT(onProcess()));
    connect(ui->radioButton_auto,SIGNAL(toggled(bool)),
            this,SLOT(onTimeEnable(bool)));
}

void DialogBatterystats::onStart()
{
    cout;
    if(ui->checkBox_reset->isEnabled()){
        emit sig_batterystat(BATTERYSTATS " --reset");
    }

    //timer count
    int time = ui->spinBox_time->text().toInt();
    if(ui->radioButton_auto->isChecked() && !isTimerEnable){
//        QString info = QString("stats start, auto stop after %1(s)").arg(time);
//        cout << info;
//        emit sig_batterystat(info);
        timer->start(time*1000);
        isTimerEnable = true;
    }else {
        cout << " Please stop manually! ";
    }
}

void DialogBatterystats::onStop()
{
    onProcess();
}

void DialogBatterystats::onTimeEnable(bool checked)
{
    cout << checked;
    ui->spinBox_time->setEnabled(checked);
}

void DialogBatterystats::onProcess()
{
    cout;
    if(isTimerEnable){
//        QMessageBox::information(this,"timer","time out ");
        timer->stop();
        isTimerEnable = false;
    }
    QString cmd = BATTERYSTATS;
    if(ui->lineEdit_path->text().isEmpty()){
        cmd += QString("%1 %2%3")
                .arg(" > batterystats.txt;")
                .arg(BUGREPORT)
                .arg(" bugreport.zip");
    }else{
        cmd += QString("%1 %2%3 %4 %5%6")
                .arg(" > ")
                .arg(ui->lineEdit_path->text())
                .arg("/batterystats.txt;")
                .arg(BUGREPORT)
                .arg(ui->lineEdit_path->text())
                .arg("/bugreport.zip");
    }

    cout << cmd;
    emit sig_batterystat(cmd);

}

void DialogBatterystats::slo_reciveMessage(QProcess::ProcessState state, QString tag)
{
    cout << state << tag;
    if(state == QProcess::ProcessState::NotRunning){
        ui->pushButton_dumpdata->setText("start");
        currentStatus = STATUS::startStats;
    }else{
        ui->pushButton_dumpdata->setText("stop");
        currentStatus = STATUS::stopStats;
    }
}

void DialogBatterystats::on_pushButton_open_clicked()
{
    Document doc;
    QString path = doc.selectDirectory("");
    ui->lineEdit_path->setText(path);
}

void DialogBatterystats::on_pushButton_dumpdata_clicked()
{
    cout;

    if(currentStatus == STATUS::startStats){
        onStart();      
    }else {
        onStop();
    }
}
