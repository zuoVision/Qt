#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QListView>
#include <QStringListModel>
#include <stdlib.h>

#define MY_TAG "mainwindow"

#define DATABASE ":/config/native_cmd_list.txt"
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
    m_userName = simpleperf->listener->m_userName.simplified();
    m_hostName = simpleperf->listener->m_hostName.simplified();
    m_totalName = m_userName.append("@").append(m_hostName).append(":~$ ");
    qDebug() <<MY_TAG <<m_totalName;
}

MainWindow::~MainWindow()
{
    delete ui;
    fileOperation->saveDataBase(DATABASE,&m_customCmdList);
}

void MainWindow::initUi()
{
    ui->checkBox_savecmd->setCheckState(Qt::Checked);
    ui->tabWidget->setTabText(0,"simpleperf");
    ui->tabWidget->setTabText(1,"CTS");
    ui->statusbar->showMessage(m_statusbarMsg);
}

void MainWindow::initEnvironment()
{
    //cmd命令行自动补全
    completer = new QCompleter(m_nativeCmdList,this);
    //最多显示数
    completer->setMaxVisibleItems(10);
    //匹配大小写不敏感
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit_cmd->setCompleter(completer);

    fileOperation->loadDataBase(DATABASE,&m_nativeCmdList);
    completer->setModel(new QStringListModel(m_nativeCmdList,this));
    qDebug()<<MY_TAG<<"[native cmd datebase]"<<m_nativeCmdList;
}

void MainWindow::initConnect()
{
    connect(&m_doc,SIGNAL(closed()),
            this,SLOT(slotReciveDocument()));
    connect(simpleperf,SIGNAL(signalToMainWindow(QString,ListenerThread::SignalType)),
            this,SLOT(slotReciveSimpleperf(QString ,ListenerThread::SignalType)));
    connect(simpleperf,SIGNAL(signalToMainWindow(QProcess::ProcessState)),
            this,SLOT(slotReciveSimpleperf(QProcess::ProcessState)));
    //cmd回车-> run button click
    connect(ui->lineEdit_cmd,SIGNAL(returnPressed()),
            ui->pushButton_run,SLOT(click()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    simpleperf->processKeyPressEvent(event);
}

void MainWindow::slotReciveDocument()
{
    ui->pushButton_doc->setEnabled(true);
}

void MainWindow::slotReciveSimpleperf(QString msg,ListenerThread::SignalType signalType)
{
    qDebug()<<MY_TAG<<"[slotReciveSimpleperf]" << msg << signalType;
    if(signalType==ListenerThread::SignalType::OUTPUT_INFO){
        ui->textEdit->append(QString("<font color=\"#0000cc\">%1</font>").arg(msg));
    }else if(signalType==ListenerThread::SignalType::ERROR_INFO){
        ui->textEdit->append(QString("<font color=\"#cc0000\">%1</font>").arg(msg));
    }else{
        ui->textEdit->append(msg);
    }
    return;
}

void MainWindow::slotReciveSimpleperf(QProcess::ProcessState newState)
{
//    qDebug()<<MY_TAG<<"[slotReciveSimpleperf] ProcessState:" << newState;
    m_processState = newState;
    if (newState == QProcess::Running){
        m_statusbarMsg = "    Process Running (you can input 'exit' to force quit!) ";
    }else if(newState == QProcess::Starting){
        m_statusbarMsg = "    Process Starting ";
    }else{
        m_statusbarMsg = "    Process Not Running ";
    }
    ui->statusbar->showMessage(m_statusbarMsg);
}


void MainWindow::on_pushButton_run_clicked()
{
    if (ui->lineEdit_cmd->text().isEmpty()
        && m_processState!=QProcess::ProcessState::NotRunning)
    {
        ui->textEdit->append(m_totalName);
        return;
    }
    //动态更新completer模型库
    if(!m_nativeCmdList.contains(ui->lineEdit_cmd->text()) &&
        ui->checkBox_savecmd->isChecked())
    {
        m_customCmdList << ui->lineEdit_cmd->text();
        m_nativeCmdList << ui->lineEdit_cmd->text();
        completer->setModel(new QStringListModel(m_nativeCmdList,this));
    }
    ui->textEdit->append(m_totalName+ui->lineEdit_cmd->text());
    simpleperf->runCmdLine(ui->lineEdit_cmd->text());
    ui->lineEdit_cmd->clear();
}

void MainWindow::on_pushButton_devices_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+ADBDEVICES);
    simpleperf->runAdbDevices(ADBDEVICES);
}

void MainWindow::on_pushButton_root_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+ADBROOT);
    simpleperf->runAdbRoot(ADBROOT);
}

void MainWindow::on_pushButton_remount_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+ADBREMOUNT);
    simpleperf->runAdbRemount(ADBREMOUNT);
}

void MainWindow::on_pushButton_oemunlock_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+ADBOEMUNLOCK);
    simpleperf->runAdbOemUnlock(ADBOEMUNLOCK);
}

void MainWindow::on_pushButton_stat_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+SIMPLEPERFSTAT);
    simpleperf->runSimpleperfStat(SIMPLEPERFSTAT);
}

void MainWindow::on_pushButton_record_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+SIMPLEPERFRECORD);
    simpleperf->runSimpleperfRecord(SIMPLEPERFRECORD);
}

void MainWindow::on_pushButton_report_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+SIMPLEPERFREPORT);
    simpleperf->runSimpleperfReport(SIMPLEPERFREPORT);
}

void MainWindow::on_pushButton_flamegraph_clicked()
{
    if(m_processState!=QProcess::ProcessState::NotRunning) return;
    ui->textEdit->append(m_totalName+"FlameGraph");
    simpleperf->runflamegraph();
}

void MainWindow::on_pushButton_doc_clicked()
{
    m_doc.show();
    m_doc.onLoadDocument(":/config/README.mk");
    ui->pushButton_doc->setEnabled(false);
}

void MainWindow::on_pushButton_cts_clicked()
{
    qDebug()<< MY_TAG << "run cts";
    QProcess *bash = new  QProcess();
    bash->setProgram(TERMINAL);
    bash->setArguments(QStringList() << "-c" << "ls");
    bash->start();
}

void MainWindow::on_comboBox_completeregular_currentIndexChanged(const int &arg1)
{
//    qDebug() << "on_comboBox_completeregular_currentIndexChanged"<<arg1;
    Qt::MatchFlags mf;
    if(arg1==0){
        mf = Qt::MatchStartsWith;
    }else if(arg1==1){
        mf = Qt::MatchContains;
    }else if(arg1==2){
        mf = Qt::MatchEndsWith;
    }else {
        mf = Qt::MatchStartsWith;
    }
    completer->setFilterMode(mf);
}
