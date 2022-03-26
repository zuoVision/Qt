#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QListView>
#include <QStringListModel>
#include <stdlib.h>

#define MY_TAG "MainWindow"

#define DATABASE ":/config/native_cmd_list.txt"
#define PYTHON2_7 "/usr/bin/python2.7"

#define ADBDEVICES      "adb devices"
#define ADBROOT         "adb root"
#define ADBREMOUNT      "adb remount"
#define ADBOEMUNLOCK    "xxxxx"

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
    m_userName = ccd->ccd_cpt->m_userName;
    qDebug() << MY_TAG << m_userName;

}

MainWindow::~MainWindow()
{
    delete ui;
    fileOperation->saveDataBase(DATABASE,&m_customCmdList);
}

void MainWindow::initUi()
{
//    ui->checkBox_savecmd->setCheckState(Qt::Checked);
    setWindowIcon(QIcon(":/icon/icon/superman.ico"));
    ui->statusbar->showMessage(m_statusbarMsg);
    ui->label_simpleperfdoc->setText(tr("<a href=\"https://android.googlesource.com/platform/system/extras/+/master/simpleperf/doc/README.md\">simpleperf参考文档"));

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
//    qDebug()<<MY_TAG<<"[native cmd datebase]"<<m_nativeCmdList;
}

void MainWindow::initConnect()
{
    //commonCommand
    connect(ccd,SIGNAL(sig_sendToMainWindow(QString)),
            this,SLOT(slo_reciveMessage(QString)));
    connect(ccd,SIGNAL(sig_sendToMainWindow(QProcess::ProcessState)),
            this,SLOT(slo_reciveMessage(QProcess::ProcessState)));

    //simpleperf
    connect(&m_doc,SIGNAL(closed()),
            this,SLOT(slotReciveDocument()));
    connect(simpleperf,SIGNAL(sig_sendToMainWindow(QString)),
            this,SLOT(slo_reciveMessage(QString)));
    connect(simpleperf,SIGNAL(sig_sendToMainWindow(QProcess::ProcessState)),
            this,SLOT(slo_reciveMessage(QProcess::ProcessState)));

    //XTS
    connect(xts,SIGNAL(sig_sendToMainWindow(QString)),
            this,SLOT(slo_reciveMessage(QString)));
    connect(xts,SIGNAL(sig_sendToMainWindow(QProcess::ProcessState)),
            this,SLOT(slo_reciveMessage(QProcess::ProcessState)));
    connect(xts,SIGNAL(sig_findCtsSuite()),
            this,SLOT(slo_showCtsSuite()));

    //cmd回车-> run button click
    connect(ui->lineEdit_cmd,SIGNAL(returnPressed()),
            ui->pushButton_run,SLOT(click()));
    connect(ui->textEdit,SIGNAL(textChanged()),
            this,SLOT(moveCursorToEnd()));

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
//    qDebug()<<MY_TAG<<"keyPressEvent";
    if((event->modifiers() == Qt::ControlModifier) &&
       (event->key() == Qt::Key_D))
    {
        qDebug()<<"CTRL + D" << ui->tabWidget->currentIndex();
        if(ui->tabWidget->currentIndex()==0)
            ccd->stopProcessor();
        if(ui->tabWidget->currentIndex()==1)
            simpleperf->stopProcessor();
        if(ui->tabWidget->currentIndex()==2)
            xts->stopProcessor();
    }
}

void MainWindow::slotReciveDocument()
{
    ui->pushButton_doc->setEnabled(true);
}

void MainWindow::slo_reciveMessage(QString msg)
{
    ui->textEdit->append(msg);
}

void MainWindow::slo_reciveMessage(QProcess::ProcessState state)
{
//    m_processState = state;
    if (state == QProcess::Running){
        m_statusbarMsg = "    Process Running (you can input 'exit' to force quit!) ";
    }else if(state == QProcess::Starting){
        m_statusbarMsg = "    Process Starting ";
    }else{
        m_statusbarMsg = "    Process Not Running ";
        if(!ui->lineEdit_cmd->text().isEmpty()) ui->lineEdit_cmd->clear();
    }
    ui->statusbar->showMessage(m_statusbarMsg);
}

void MainWindow::slo_showCtsSuite()
{

    ui->comboBox_ctssuite->addItems(xts->m_ctsSuite);


}

void MainWindow::moveCursorToEnd()
{
    //移动光标到末尾
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
}

void MainWindow::on_pushButton_run_clicked()
{
    //动态更新completer模型库
    if(!m_nativeCmdList.contains(ui->lineEdit_cmd->text()) &&
        ui->checkBox_savecmd->isChecked())
    {
        m_customCmdList << ui->lineEdit_cmd->text();
        m_nativeCmdList << ui->lineEdit_cmd->text();
        completer->setModel(new QStringListModel(m_nativeCmdList,this));
    }
    ccd->runCommand(ui->lineEdit_cmd->text());
    ui->lineEdit_cmd->clear();
}

void MainWindow::on_pushButton_devices_clicked()
{
    ccd->runCommand(ADBDEVICES);
}

void MainWindow::on_pushButton_root_clicked()
{
    ccd->runCommand(ADBROOT);
}

void MainWindow::on_pushButton_remount_clicked()
{
    ccd->runCommand(ADBREMOUNT);
}

void MainWindow::on_pushButton_oemunlock_clicked()
{
    ccd->runCommand(ADBOEMUNLOCK);
}

void MainWindow::on_pushButton_stat_clicked()
{
    simpleperf->runStat();
}

void MainWindow::on_pushButton_record_clicked()
{
    simpleperf->runRecord();
}

void MainWindow::on_pushButton_report_clicked()
{
    simpleperf->runReport();
}

void MainWindow::on_pushButton_flamegraph_clicked()
{
    simpleperf->runFlamegraph();
}

void MainWindow::on_pushButton_doc_clicked()
{
    m_doc.show();
    m_doc.onLoadDocument(":/config/README.mk");
    ui->pushButton_doc->setEnabled(false);
}

void MainWindow::on_pushButton_cts_clicked()
{
    xts->runCts();
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

