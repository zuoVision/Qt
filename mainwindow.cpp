#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    init_process();

    connect(p , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
    connect(p , SIGNAL(readyReadStandardError()) , this , SLOT(on_readerror()));
    //cmd回车-> run button click
    connect(ui->lineEdit_cmd,SIGNAL(returnPressed()),ui->pushButton_run,SLOT(click()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_process()
{
    QProcessEnvironment env;
//    env.insert("PYTHONPATH", "D:/ProgramData/Anaconda3/envs/pytorch_cuda/Lib");
//    env.insert("PYTHONHOME", "D:/ProgramData/Anaconda3/envs/pytorch_cuda");
//    p->setProcessEnvironment(env);
//    QString pythonPath = "";

}

void MainWindow::excuteCmd(QString filePath, QStringList params)
{
    p->start(filePath,params);
    p->waitForFinished(-1);
    QString p_stdoutput = p->readAllStandardOutput();
    QString p_stderr = p->readAllStandardError();
    ui->textEdit->append(p_stdoutput);
    ui->textEdit->append(p_stderr);
}

void MainWindow::on_pushButton_run_clicked()
{
    if (ui->lineEdit_cmd->text().isEmpty()){
        ui->textEdit->append("Please input your command !");
        return;
    }
    ui->textEdit->append("zuozhe$ "+ui->lineEdit_cmd->text());
    ui->lineEdit_cmd->clear();


}

void MainWindow::on_pushButton_stat_clicked()
{
    p = new QProcess();
    QStringList cmd;
    cmd  << "test.py" ;
    p->start(" /usr/bin/python3.8",cmd );
    qDebug() << cmd;
    p->waitForStarted();
    p->waitForFinished();
    ui->textEdit->append(p->readAllStandardOutput());


}

void MainWindow::on_pushButton_record_clicked()
{
    QStringList cmd;
    cmd << "E:/Workstation/QtProject/build-one_click_generation-Desktop_Qt_5_12_9_MinGW_64_bit-Debug/debug/test.py";
    p->start("E:/Application/Install/x86_64-8.1.0-release-posix-seh-rt_v6-rev0/mingw64/opt/bin/python2.7.exe",
                      cmd);

    //等待程序执行完
    p->waitForReadyRead();
//    QByteArray arr =  p->readAllStandardOutput();

}

void MainWindow::on_pushButton_flamegraph_clicked()
{
    QString fileName = QCoreApplication::applicationDirPath() + "/graph111.svg";
    if(QDesktopServices::openUrl(fileName)){
        ui->textEdit->append("Open FlameGraph Success !\n"+fileName);
    }else {
        ui->textEdit->append("Open FlameGraph Failed !!!\n "+fileName);
    }

}

void MainWindow::on_readoutput()
{
    ui->textEdit->append(p->readAllStandardOutput().data());
}

void MainWindow::on_readerror()
{
    QMessageBox::critical(0,"Error",p->readAllStandardError().data());
}
