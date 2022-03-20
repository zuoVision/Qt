#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QListView>
#include <QStringListModel>
#include <stdlib.h>

#define MY_TAG "mainwindow"

#define DATABASE "cmd_database.txt"
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
    initUi();
    initEnvironment();
    initConnect();
    qDebug() << MY_TAG  <<"[MainWindow]"
             << QThread::currentThreadId();
//    QThread::sleep(2);
    m_hostName = simpleperf->listener->m_hostName;
    qDebug() <<MY_TAG <<m_hostName;

}

MainWindow::~MainWindow()
{
    delete ui;
    fileOperation->saveDataBase(DATABASE,&m_cmdList);
}

void MainWindow::initUi()
{
//    searchBar();
    ui->tabWidget->setTabText(0,"simpleperf");
    ui->tabWidget->setTabText(1,"CTS");

}

void MainWindow::initEnvironment()
{
    //cmd命令行自动补全
    completer = new QCompleter(m_cmdList,this);
    //最多显示数
    completer->setMaxVisibleItems(6);
    //过滤方式==》只要包含string 即可
    completer->setFilterMode(Qt::MatchStartsWith);
    //匹配大小写不敏感
    completer->setCaseSensitivity(Qt::CaseInsensitive);
//    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->lineEdit_cmd->setCompleter(completer);

    fileOperation->loadDataBase(DATABASE,&m_cmdList);
    completer->setModel(new QStringListModel(m_cmdList,this));
    qDebug()<<MY_TAG<<"[cmd datebase]"<<m_cmdList;
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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    simpleperf->processKeyPressEvent(event);
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
//    qDebug()<<MY_TAG<<"[slotReciveSimpleperf] ProcessState:" << newState;
    if (newState == QProcess::Running){
        m_statusbarMsg = "    Process Running ";
    }else if(newState == QProcess::Starting){
        m_statusbarMsg = "    Process Starting ";
    }else{
        m_statusbarMsg = "    Process Not Running";
    }
    ui->statusbar->showMessage(m_statusbarMsg);
}


void MainWindow::on_pushButton_run_clicked()
{
    if (ui->lineEdit_cmd->text().isEmpty()){
        ui->textEdit->append("Please input your command !");
        return;
    }
    //动态更新completer模型库
    if(!m_cmdList.contains(ui->lineEdit_cmd->text())){
        m_cmdList << ui->lineEdit_cmd->text();
        completer->setModel(new QStringListModel(m_cmdList,this));
    }
    ui->textEdit->append(m_hostName+ui->lineEdit_cmd->text());
//    simpleperf->runCmdLine(ui->lineEdit_cmd->text());
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
