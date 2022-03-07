#include "mainwindow.h"
#include "ui_mainwindow.h"

#define PYTHON2_7 "/usr/bin/python2.7"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(p , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
    connect(p , SIGNAL(readyReadStandardError()) , this , SLOT(on_readerror()));
    //cmd回车-> run button click
    connect(ui->lineEdit_cmd,SIGNAL(returnPressed()),ui->pushButton_run,SLOT(click()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::excuteCmd(QStringList params)
{
    QString program;
    program = "bash";
    p->start(program,params);
    //等待程序执行完
    p->waitForReadyRead();
    ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    cmd.clear();
}

void MainWindow::excuteCmd(QString filePath, QStringList params)
{
    p->start(filePath,params);
    //等待程序执行完
    p->waitForReadyRead();
    ui->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    cmd.clear();
}

void MainWindow::on_readoutput()
{
    ui->textEdit->append(p->readAllStandardOutput().data());
}

void MainWindow::on_readerror()
{
    ui->textEdit->append(p->readAllStandardError().data());
}

void MainWindow::on_pushButton_run_clicked()
{
    if (ui->lineEdit_cmd->text().isEmpty()){
        ui->textEdit->append("Please input your command !");
        return;
    }
    ui->textEdit->append(QCoreApplication::applicationFilePath()+"$ "+ui->lineEdit_cmd->text());
    QStringList params;
    params << "-c" << ui->lineEdit_cmd->text();
    excuteCmd(params);

    ui->lineEdit_cmd->clear();
}

void MainWindow::on_pushButton_devices_clicked()
{

    cmd << "-c" << "adb devices";
    excuteCmd(cmd);
}

void MainWindow::on_pushButton_root_clicked()
{
    cmd << "-c" << "adb root";
    excuteCmd(cmd);
}

void MainWindow::on_pushButton_remount_clicked()
{
    cmd << "-c" << "adb remount";
    excuteCmd(cmd);
}

void MainWindow::on_pushButton_oemunlock_clicked()
{
    ui->textEdit->append("please make sure 'oem unlocking' set on ...");

    cmd << "-c" << "adb reboot bootloader;"
                      "fastboot flashing unlock;"
                      "fastboot reboot ;"
                      "adb wait-for-device;"
                      "adb root;"
                      "adb disable-verity;"
                      "adb remount;"
                      "adb reboot ;"
                      "adb wait-for-device;"
                      "adb root,adb remount";
    excuteCmd(cmd);
}

void MainWindow::on_pushButton_stat_clicked()
{
    ui->textEdit->append("<font color=\"#0000ff\">********** Simpleperf Stat **********</font> ");
    ui->textEdit->append("test");
}

void MainWindow::on_pushButton_record_clicked()
{
    ui->textEdit->append("<font color=\"#0000ff\">********** Simpleperf Record **********</font> ");

// 1. python
//    QStringList cmd ;
//    cmd << QCoreApplication::applicationDirPath() + "/scripts/app_profiler.py"
//        << "-p" << "com.tcl.camera";
//    excuteCmd(PYTHON2_7,cmd);

//    2.bash

    cmd <<"-c" << "python scripts/app_profiler.py -p com.tcl.camera";
    excuteCmd(cmd);


    cmd << "-c" << "adb pull /data/local/tmp/perf.data";
    excuteCmd(cmd);
    ui->textEdit->append("<font color=\"#00cc00\">Simpleperf Record Finished\n</font>");
}

void MainWindow::on_pushButton_report_clicked()
{
    ui->textEdit->append("<font color=\"#0000ff\">********** Simpleperf Report **********</font> ");

    cmd <<"-c" << "python scripts/report_sample.py > out.perf";
    excuteCmd(cmd);
    ui->textEdit->append(QCoreApplication::applicationDirPath()+"out.perf Done!");
    ui->textEdit->append("<font color=\"#00cc00\">Simpleperf Report Finished\n</font> ");
}

void MainWindow::on_pushButton_flamegraph_clicked()
{
    ui->textEdit->append("<font color=\"#0000ff\">********** FlameGraph **********\n</font> ");

    QString graphName = "/graph.svg";
    QString fileName = QCoreApplication::applicationDirPath() + graphName;


    cmd <<"-c" << "FlameGraph/stackcollapse-perf.pl out.perf > out.folded";
    excuteCmd(cmd);

    cmd <<"-c" << "FlameGraph/flamegraph.pl out.folded > graph.svg";
    excuteCmd(cmd);

    if(QDesktopServices::openUrl(fileName)){
        ui->textEdit->append(fileName);
        ui->textEdit->append("<font color=\"#00cc00\">FlameGraph Opened  </font>");
    }else {
        ui->textEdit->append("<font color=\"#ee0000\">Open FlameGraph Failed !</font> ");
    }

}
