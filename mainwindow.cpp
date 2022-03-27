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
#define cout            qDebug() << MY_TAG /*<< __BASE_FILE__ << __LINE__*/ << __FUNCTION__

#define DATABASE        ":/config/native_cmd_list.txt"
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
    ui->statusbar->showMessage(m_statusbarMsg);
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
    completer->setFilterMode(mf);
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
    QDesktopServices::openUrl(QUrl(ui->comboBox_ctssuite->currentText()+"../../../logs/latest/"));
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
    readXml();
    insertDataToTable();
}

void MainWindow::readXml()
{
    QFile file("test_result.xml");
    QString nodename;
    QString output;
    if(!file.exists()){
        QMessageBox::warning(this,"warning","file not found!");
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"warning","file open failed!");
        return;
    }
    QXmlStreamReader xmlreader(&file);
    while (!xmlreader.atEnd() || !xmlreader.hasError()) {
        xmlreader.readNextStartElement();
        nodename = xmlreader.name().toString();
        if(nodename == "Module" && xmlreader.isStartElement()){
            m_modulename = xmlreader.attributes().value("name").toString();
            m_totalTests = xmlreader.attributes().value("total_tests").toString();
            m_pass       = xmlreader.attributes().value("pass").toString();
//            QMessageBox::information(this,"Result",
//                                     QString("Module :%1\nTotal Tests :%2\nPass:%3").
//                                     arg(xmlreader.attributes().value("name").toString()).
//                                     arg(xmlreader.attributes().value("total_tests").toString()).
//                                     arg(xmlreader.attributes().value("pass").toString()));
            while (!(nodename == "Module" && xmlreader.isEndElement())) {// module not end
                xmlreader.readNextStartElement();
                nodename = xmlreader.name().toString();
                if(nodename == "TestCase" && xmlreader.isStartElement()){
//                    cout << xmlreader.attributes().value("name").toString();
                    QString testcase = xmlreader.attributes().value("name").toString();
                    while (!(nodename == "TestCase" && xmlreader.isEndElement())) {
                        xmlreader.readNextStartElement();
                        nodename = xmlreader.name().toString();
                        if(nodename == "Test" && xmlreader.isStartElement()){
                            QString name    = xmlreader.attributes().value("name").toString();
                            QString result  = xmlreader.attributes().value("result").toString();
                            m_test   << testcase+"#"+name;
                            m_result << result;
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::insertDataToTable()
{
    QStringList *list = new QStringList();
    QString test,result,fail;
    fail = QString::number(m_totalTests.toInt() - m_pass.toInt());
    test = QString("Test(%1)").arg(m_totalTests);
    result = QString("Result(pass %1 fail %2)").arg(m_pass).arg(fail);
    *list << test << result;
    ui->tableWidget_xts->setHorizontalHeaderLabels(*list);
    ui->tableWidget_xts->setRowCount(m_totalTests.toInt());
    for(int row=0;row<ui->tableWidget_xts->rowCount();row++){
        ui->tableWidget_xts->setItem(row,0,new QTableWidgetItem(m_test[row]));
        ui->tableWidget_xts->setItem(row,1,new QTableWidgetItem(m_result[row]));
    }
}
