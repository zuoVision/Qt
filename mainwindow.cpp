#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QListView>
#include <stdlib.h>

#define MY_TAG "mainwindow"
#define PYTHON2_7 "/usr/bin/python2.7"
#define GNOME "/etc/gnome"
#define TERMINAL "/usr/bin/terminator"

#define ADBDEVICES      "adb devices"
#define ADBROOT         "adb root"
#define ADBREMOUNT      "adb remount"
#define ADBOEMUNLOCK    "xxxxx"

#define SIMPLEPERFSTAT   "XXXX"
#define SIMPLEPERFRECORD "python scripts/app_profiler.py -p com.tcl.camera"
#define SIMPLEPERFREPORT "python scripts/report_sample.py > out.perf"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
    initEnv();
    initConnect();
    qDebug() << MY_TAG  <<"[MainWindow]"
             << QThread::currentThreadId();
    m_hostName = simpleperf->listener->m_hostName;


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initEnv()
{
    searchBar();
    ui->tabWidget->setTabText(0,"simpleperf");
    ui->tabWidget->setTabText(1,"CTS");
}

void MainWindow::initConnect()
{
    connect(simpleperf,SIGNAL(signalToMainWindow(QString)),
            this,SLOT(slotReciveSimpleperf(QString)));
    connect(simpleperf,SIGNAL(signalToMainWindow(QProcess::ProcessState)),
            this,SLOT(slotReciveSimpleperf(QProcess::ProcessState)));
    //cmd回车-> run button click
    connect(ui->lineEdit_cmd,SIGNAL(returnPressed()),
            ui->pushButton_run,SLOT(click()));
}

void MainWindow::searchBar()
{
    ui->comboBox->setView(new QListView);
    ui->comboBox->setLineEdit(ui->lineEdit_search);
    ui->comboBox->setEditable(true);
    ui->comboBox->setMaxVisibleItems(6);
    ui->comboBox->lineEdit()->setPlaceholderText("search");
//    ui->lineEdit_search->setPlaceholderText("input");
//    ui->listView_cmdRcord.set
}



void MainWindow::slotReciveSimpleperf(QString msg)
{
//    qDebug()<<MY_TAG<<"[slotReciveSimpleperf]";
    if(!msg.isEmpty()){
        ui->textEdit->append(msg);
    }
    return;
}

void MainWindow::slotReciveSimpleperf(QProcess::ProcessState newState)
{
    qDebug()<<MY_TAG<<"[slotReciveSimpleperf] ProcessState:" << newState;
    QString statusbarMsg;
    if (newState == QProcess::Running){
        statusbarMsg = "    Process Running ";
    }else if(newState == QProcess::Starting){
        statusbarMsg = "    Process Starting ";
    }else{
        statusbarMsg = "    Process Not Running";
    }

    ui->statusbar->showMessage(statusbarMsg);

}


void MainWindow::on_pushButton_run_clicked()
{
    if (ui->lineEdit_cmd->text().isEmpty()){
        ui->textEdit->append("Please input your command !");
        return;
    }
    m_cmdVector << ui->lineEdit_cmd->text();

    ui->textEdit->append(m_hostName+ui->lineEdit_cmd->text());
    simpleperf->runCmdLine(ui->lineEdit_cmd->text());
    ui->lineEdit_cmd->clear();
}

void MainWindow::on_pushButton_devices_clicked()
{
    ui->textEdit->append(m_hostName+ADBDEVICES);
    simpleperf->runAdbDevices(ADBDEVICES);
}

void MainWindow::on_pushButton_root_clicked()
{
    ui->textEdit->append(m_hostName+ADBROOT);
    simpleperf->runAdbRoot(ADBROOT);
}

void MainWindow::on_pushButton_remount_clicked()
{
    ui->textEdit->append(m_hostName+ADBREMOUNT);
    simpleperf->runAdbRemount(ADBREMOUNT);
}

void MainWindow::on_pushButton_oemunlock_clicked()
{
    ui->textEdit->append(m_hostName+ADBOEMUNLOCK);
    simpleperf->runAdbOemUnlock(ADBOEMUNLOCK);
}

void MainWindow::on_pushButton_stat_clicked()
{
    ui->textEdit->append(m_hostName+SIMPLEPERFSTAT);
    simpleperf->runSimpleperfStat(SIMPLEPERFSTAT);
}

void MainWindow::on_pushButton_record_clicked()
{
    ui->textEdit->append(m_hostName+SIMPLEPERFRECORD);
    simpleperf->runSimpleperfRecord(SIMPLEPERFRECORD);
}

void MainWindow::on_pushButton_report_clicked()
{
    ui->textEdit->append(m_hostName+SIMPLEPERFREPORT);
    simpleperf->runSimpleperfReport(SIMPLEPERFREPORT);
}

void MainWindow::on_pushButton_flamegraph_clicked()
{
    ui->textEdit->append(m_hostName+"FlameGraph");
    simpleperf->runflamegraph();
}

void MainWindow::on_pushButton_doc_clicked()
{
    m_doc.show();
    m_doc.onLoadDocument("README.md");
}

void MainWindow::on_pushButton_cts_clicked()
{
    qDebug()<< MY_TAG << "run cts";
    QProcess *bash = new  QProcess();
    bash->setProgram(TERMINAL);
    bash->setArguments(QStringList() << "-c" << "ls");
    bash->start();
}
