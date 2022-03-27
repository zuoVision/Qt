#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QStringListModel>
#include <QXmlStreamReader>
#include <stdlib.h>

#define MY_TAG          "MainWindow"
#define cout            qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]"

#define DATABASE        ":/config/native_cmd_list.txt"
#define CTSTEST         ":/config/cts_test_list.txt"
#define PYTHON2_7       "/usr/bin/python2.7"

#define ADBDEVICES      "adb devices"
#define ADBROOT         "adb root"
#define ADBREMOUNT      "adb remount"
#define ADBOEMUNLOCK    "xxxxx"

#define TESTRESULT      "test_result.xml"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
    initUi();
    initEnvironment();
    initConnect();
    cout << QThread::currentThreadId();
    m_userName = ccd->ccd_cpt->m_userName;
    cout << m_userName;

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
//    ui->statusbar->showMessage(m_statusbarMsg);

    m_ccd_status = new QLabel("ccd",this);
    m_sim_status = new QLabel("sim",this);
    m_xts_status = new QLabel("xts",this);
    m_ccd_status->setMinimumWidth(100);
    m_ccd_status->setMinimumWidth(100);
    m_ccd_status->setMinimumWidth(100);
    ui->statusbar->addWidget(m_ccd_status);
    ui->statusbar->addWidget(m_sim_status);
    ui->statusbar->addWidget(m_xts_status);

    ui->textEdit->setReadOnly(true);
    ui->label_simpleperfdoc->setText(tr("<a href=\"https://android.googlesource.com/platform/system/extras/+/master/simpleperf/doc/README.md\">simpleperf参考文档"));

    ui->tableWidget_xts->verticalHeader()->show();
    ui->tableWidget_xts->horizontalHeader()->show();
    //zi shi ying kuan du
    ui->tableWidget_xts->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget_xts->setHorizontalHeaderLabels(QStringList()<<"Tests"<<"Result"<<"Resolution");
    ui->tableWidget_xts->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightgreen;}");
    ui->tableWidget_xts->setSortingEnabled(true);
    ui->tableWidget_xts->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
}

void MainWindow::initEnvironment()
{
    //cmd命令行自动补全
    cmd_completer = new QCompleter();
    //最多显示数
    cmd_completer->setMaxVisibleItems(10);
    //匹配大小写不敏感
    cmd_completer->setCaseSensitivity(Qt::CaseInsensitive);
    fileOperation->loadDataBase(DATABASE,&m_nativeCmdList);
    cmd_completer->setModel(new QStringListModel(m_nativeCmdList,this));
    ui->lineEdit_cmd->setCompleter(cmd_completer);

    //cts test自动补全
    test_completer = new QCompleter();
    test_completer->setMaxVisibleItems(10);
    test_completer->setCaseSensitivity(Qt::CaseInsensitive);
    test_completer->setFilterMode(Qt::MatchContains);
    fileOperation->loadDataBase(":/config/cts_test_list.txt",&m_ctsTestList);
    test_completer->setModel(new QStringListModel(m_ctsTestList,this));
    ui->lineEdit_ctstest->setCompleter(test_completer);
}

void MainWindow::initConnect()
{
    //help
    connect(ui->actionDocument,SIGNAL(triggered()),
            this,SLOT(slo_openDocument()));

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
        cout <<"CTRL + D" << ui->tabWidget->currentIndex();
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
    ui->actionDocument->setEnabled(true);
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
//    ui->statusbar->showMessage(m_statusbarMsg);
}

void MainWindow::slo_showCtsSuite()
{
    ui->comboBox_ctssuite->addItems(xts->m_ctsSuite);
}

void MainWindow::slo_openDocument()
{
    m_doc.show();
    m_doc.onLoadDocument(":/config/README.mk");
    ui->actionDocument->setEnabled(false);
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
    //动态更新cmd_completer模型库
    if(!m_nativeCmdList.contains(ui->lineEdit_cmd->text()) &&
        ui->checkBox_savecmd->isChecked())
    {
        m_customCmdList << ui->lineEdit_cmd->text();
        m_nativeCmdList << ui->lineEdit_cmd->text();
        cmd_completer->setModel(new QStringListModel(m_nativeCmdList,this));
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

void MainWindow::on_pushButton_runcts_clicked()
{
    QString m_ctsSuite      = ui->comboBox_ctssuite->currentText();
    QString m_ctsCommand    = ui->comboBox_ctscommand->currentText();
    QString m_ctsModule     = ui->comboBox_ctsmodule->currentText();
    QString m_ctsTest       = ui->lineEdit_ctstest->text();
    xts->runCts(m_ctsSuite,m_ctsCommand,m_ctsModule,m_ctsTest);
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
    cmd_completer->setFilterMode(mf);
}

void MainWindow::on_pushButton_loadctssuite_clicked()
{
   ui->comboBox_ctssuite->insertItem(-1,m_doc.openFile());
   ui->comboBox_ctssuite->setCurrentIndex(0);
}

void MainWindow::on_pushButton_log_clicked()
{
    if (ui->comboBox_ctssuite->currentText().isEmpty())
    {
        QMessageBox::warning(this,"warning","please choose cts suite ...");
        return;
    }
    QString path = ui->comboBox_ctssuite->currentText()+"../../../logs/latest/";
    if(!QDesktopServices::openUrl(QUrl(path))){
        QMessageBox::warning(this,"warning",QString("open folder failed#{%1}").arg(path));
    }

}

void MainWindow::on_pushButton_result_clicked()
{

    if (ui->comboBox_ctssuite->currentText().isEmpty())
    {
        QMessageBox::warning(this,"warning","please choose cts suite ...");
        return;
    }
#if 0
    QDesktopServices::openUrl(QUrl(ui->comboBox_ctssuite->currentText()+"../../../results/latest/test_result.html"));
#endif
    QFile file("test_result.xml");
    if(!file.exists()){
        QMessageBox::warning(this,"warning","file not found!");
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"warning","file open failed!");
        return;
    }
    if(fileOperation->readXml(&file)) insertDataToTable();
}

//丢弃
//void MainWindow::readXml()
//{
//    QFile file("test_result.xml");
//    QString nodename;
//    QString output;
//    if(!file.exists()){
//        QMessageBox::warning(this,"warning","file not found!");
//        return;
//    }
//    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        QMessageBox::warning(this,"warning","file open failed!");
//        return;
//    }
//    QXmlStreamReader xmlreader(&file);
//    while (!xmlreader.atEnd() || !xmlreader.hasError()) {
//        xmlreader.readNextStartElement();
//        nodename = xmlreader.name().toString();
//        if(nodename == "Module" && xmlreader.isStartElement()){
//            m_modulename = xmlreader.attributes().value("name").toString();
//            m_totalTests = xmlreader.attributes().value("total_tests").toString();
//            m_pass       = xmlreader.attributes().value("pass").toString();
////            QMessageBox::information(this,"Result",
////                                     QString("Module :%1\nTotal Tests :%2\nPass:%3").
////                                     arg(xmlreader.attributes().value("name").toString()).
////                                     arg(xmlreader.attributes().value("total_tests").toString()).
////                                     arg(xmlreader.attributes().value("pass").toString()));
//            while (!(nodename == "Module" && xmlreader.isEndElement())) {// module not end
//                xmlreader.readNextStartElement();
//                nodename = xmlreader.name().toString();
//                if(nodename == "TestCase" && xmlreader.isStartElement()){
////                    cout << xmlreader.attributes().value("name").toString();
//                    QString testcase = xmlreader.attributes().value("name").toString();
//                    while (!(nodename == "TestCase" && xmlreader.isEndElement())) {
//                        xmlreader.readNextStartElement();
//                        nodename = xmlreader.name().toString();
//                        if(nodename == "Test" && xmlreader.isStartElement()){
//                            QString name    = xmlreader.attributes().value("name").toString();
//                            QString result  = xmlreader.attributes().value("result").toString();
//                            m_test   << testcase+"#"+name;
//                            m_result << result;
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

void MainWindow::insertDataToTable()
{
    QStringList *list = new QStringList();
    QString test,result,fail;
    fail = QString::number(fileOperation->m_totalTests.toInt() - fileOperation->m_pass.toInt());
    test = QString("Test(%1)").arg(fileOperation->m_totalTests);
    result = QString("Result(pass %1 fail %2)").arg(fileOperation->m_pass).arg(fail);
    *list << test << result;
    ui->tableWidget_xts->setHorizontalHeaderLabels(*list);
    ui->tableWidget_xts->setRowCount(fileOperation->m_totalTests.toInt());
    for(int row=0;row<ui->tableWidget_xts->rowCount();row++){
        ui->tableWidget_xts->setItem(row,0,new QTableWidgetItem(fileOperation->m_test[row]));
        ui->tableWidget_xts->setItem(row,1,new QTableWidgetItem(fileOperation->m_result[row]));
    }
}
