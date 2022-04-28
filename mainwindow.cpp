#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cmd.h"
#include <QDesktopServices>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QStringListModel>
#include <QTableView>
#include <QXmlStreamReader>
#include <QVBoxLayout>

#include <QDir>
#include <QTime>
#include <stdlib.h>

#define MY_TAG          "MainWindow"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

#define DATABASE        "config/native_cmd_list.txt"
#define CTSTESTLIST     ":/config/config/cts_test_list.txt"
#define CTSRESULOTION   ":/config/config/cts_resulotion.csv"
#define TESTRESULT      ":/config/config/test_result.xml"

#define SIMPLEPERFDOC   "<a href=\"https://android.googlesource.com/platform/system/extras/+/master/simpleperf/doc/README.md\">simpleperf参考文档"

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

    //status bar
    m_ccd_status = new QLabel("command",this);
    m_sim_status = new QLabel("simpleperf",this);
    m_xts_status = new QLabel("xts",this);
    m_ccd_status->setFixedSize(20,20);
    m_sim_status->setFixedSize(20,20);
    m_xts_status->setFixedSize(20,20);
    led_red = new QPixmap(":/icon/icon/red.ico");
    led_green = new QPixmap(":/icon/icon/green.ico");
    led_yellow = new QPixmap(":/icon/icon/yellow.ico");
    led_grey = new QPixmap(":/icon/icon/grey.ico");
    led_blue = new QPixmap(":/icon/icon/blue.ico");
    led_red->scaled(m_ccd_status->size(), Qt::KeepAspectRatio);
    led_green->scaled(m_ccd_status->size(), Qt::KeepAspectRatio);
    led_yellow->scaled(m_ccd_status->size(), Qt::KeepAspectRatio);
    m_ccd_status->setScaledContents(true);
    m_sim_status->setScaledContents(true);
    m_xts_status->setScaledContents(true);
    m_ccd_status->setPixmap(*led_grey);
    m_sim_status->setPixmap(*led_grey);
    m_xts_status->setPixmap(*led_grey);
    ui->statusbar->addWidget(m_ccd_status);
    ui->statusbar->addWidget(m_sim_status);
    ui->statusbar->addWidget(m_xts_status);

    ui->textEdit->setReadOnly(true);
    ui->label_simpleperfdoc->setText(SIMPLEPERFDOC);

    mtv->setParent(ui->tab_xts);
    ui->tab_xts->layout()->addWidget(mtv);

    //find lineEdit
    lineEdit_filter = new QLineEdit(ui->dockWidget_output);
    lineEdit_filter->setPlaceholderText("Filter");
    lineEdit_filter->setClearButtonEnabled(true);
    lineEdit_filter->hide();
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
    fileOperation->loadDataBase(CTSTESTLIST,&m_ctsTestList);
    test_completer->setModel(new QStringListModel(m_ctsTestList,this));
    ui->lineEdit_ctstest->setCompleter(test_completer);

    //
    loadCtsResulotion();
}

void MainWindow::initConnect()
{
    //help
    connect(ui->actionDocument,SIGNAL(triggered()),
            this,SLOT(slo_openDocument()));

    //commonCommand
    connect(ccd,SIGNAL(sig_sendToMainWindow(QString)),
            this,SLOT(slo_reciveMessage(QString)));
    connect(ccd,SIGNAL(sig_sendToMainWindow(QProcess::ProcessState,QString)),
            this,SLOT(slo_reciveMessage(QProcess::ProcessState,QString)));

    //simpleperf
    connect(&m_doc,SIGNAL(closed()),
            this,SLOT(slotReciveDocument()));
    connect(simpleperf,SIGNAL(sig_sendToMainWindow(QString)),
            this,SLOT(slo_reciveMessage(QString)));
    connect(simpleperf,SIGNAL(sig_sendToMainWindow(QProcess::ProcessState,QString)),
            this,SLOT(slo_reciveMessage(QProcess::ProcessState,QString)));

    //XTS
    connect(this,SIGNAL(sig_sendToXts(QString)),
            xts,SLOT(slo_reciveMainWindow(QString)));
    connect(xts,SIGNAL(sig_sendToMainWindow(QString)),
            this,SLOT(slo_reciveMessage(QString)));
    connect(xts,SIGNAL(sig_sendToMainWindow(QProcess::ProcessState,QString)),
            this,SLOT(slo_reciveMessage(QProcess::ProcessState,QString)));
    connect(xts,SIGNAL(sig_showCtsResult()),
            this,SLOT(slo_showCtsResult()));

    //cmd回车-> run button click
    connect(ui->lineEdit_cmd,SIGNAL(returnPressed()),
            this,SLOT(on_pushButton_run_clicked()));
    connect(ui->textEdit,SIGNAL(textChanged()),
            this,SLOT(moveCursorToEnd()));
    connect(lineEdit_filter,SIGNAL(textChanged(const QString)),
            this,SLOT(onTextFilter()));

    //battery historian
    connect(batterystats,SIGNAL(sig_batterystat(QString)),
            this,SLOT(slo_batterystats(QString)));
    connect(this,SIGNAL(sig_sendToBatterystats(QProcess::ProcessState,QString)),
            batterystats,SLOT(slo_reciveMessage(QProcess::ProcessState,QString)));

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //TODO:ctrl + D no response
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
    if(event->modifiers() == Qt::ControlModifier &&
       event->key() == Qt::Key_F){
        cout <<"CTRL + F";
        onContrlSearchBar();
    }
}

//void MainWindow::resizeEvent(QResizeEvent *event)
//{
        //window size change listener
//}

void MainWindow::onContrlSearchBar()
{
    if(lineEdit_filter->isHidden()){
        lineEdit_filter->setGeometry(60,0,100,25);
        lineEdit_filter->show();
        lineEdit_filter->setFocus();
    }else {
        lineEdit_filter->clear();
        lineEdit_filter->hide();
        onTextFilter();
    }
}

void MainWindow::slotReciveDocument()
{
    ui->actionDocument->setEnabled(true);
}

void MainWindow::slo_batterystats(QString cmd)
{
    cout << cmd;
    ccd->runCommand(cmd);
}

void MainWindow::slo_reciveMessage(QString msg)
{
    if(msg.startsWith("Warning")){
        QMessageBox::warning(this,"warning",msg);
        return;
    }
    ui->textEdit->append(msg);
}

void MainWindow::slo_reciveMessage(QProcess::ProcessState state,QString tag)
{
    if (state == QProcess::Running){
        if (tag == "CommonCommand"){
            m_ccd_status->setPixmap(*led_green);
            emit sig_sendToBatterystats(state,tag);
        }
        if (tag == "Simpleperf") m_sim_status->setPixmap(*led_green);
        if (tag == "Xts") m_xts_status->setPixmap(*led_green);
//            m_statusbarMsg = "    Process Running (you can input 'exit' to force quit!) ";
    }else if(state == QProcess::Starting){
        if (tag == "CommonCommand") {
            m_ccd_status->setPixmap(*led_blue);
            emit sig_sendToBatterystats(state,tag);
        }
        if (tag == "Simpleperf") m_sim_status->setPixmap(*led_blue);
        if (tag == "Xts") m_xts_status->setPixmap(*led_blue);
//            m_statusbarMsg = "    Process Starting ";
    }else{
        if (tag == "CommonCommand") {
            m_ccd_status->setPixmap(*led_grey);
            emit sig_sendToBatterystats(state,tag);
        }
        if (tag == "Simpleperf") m_sim_status->setPixmap(*led_grey);
        if (tag == "Xts") m_xts_status->setPixmap(*led_grey);
//            m_statusbarMsg = "    Process Not Running ";
        if(!ui->lineEdit_cmd->text().isEmpty()) ui->lineEdit_cmd->clear();
    }
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
    if(ui->lineEdit_cmd->text()=="clear") {
        ui->textEdit->clear();
        ui->lineEdit_cmd->clear();
        return;
    }
    ccd->runCommand(ui->lineEdit_cmd->text());
    ui->lineEdit_cmd->clear();
}

void MainWindow::on_pushButton_devices_clicked()
{
    cout;
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
    QMessageBox::information(this,"Info","Make sure 'OEM unlocking' turn on!");
    ccd->runCommand(ADBOEMUNLOCK);
}

void MainWindow::on_pushButton_list_clicked()
{
    cout;
    simpleperf->runList();
}

void MainWindow::on_pushButton_stat_clicked()
{
//    cout;
    if(getStatParams()){
        simpleperf->runStat(&m_statParams);
    }else{
        simpleperf->runStat();
    }
}

void MainWindow::on_pushButton_record_clicked()
{
//    cout;
    if(getRecordParams()){
        simpleperf->runRecord(&m_recordParams);
    }else{
        simpleperf->runRecord();
    }
}

void MainWindow::on_pushButton_report_clicked()
{
    simpleperf->runReport();
}

void MainWindow::on_pushButton_flamegraph_clicked()
{
    simpleperf->runFlamegraph();
}

void MainWindow::on_pushButton_quickgeneration_clicked()
{
    simpleperf->runQuickGeneration();
}

void MainWindow::on_pushButton_statclear_clicked()
{
    ui->lineEdit_statpid->clear();
    ui->lineEdit_stattid->clear();
    ui->spinBox_statduration->setValue(10);
    ui->checkBox_statsw->setCheckState(Qt::CheckState::Unchecked);
    ui->lineEdit_statevent->clear();
    ui->lineEdit_statcpu->clear();
}

void MainWindow::on_pushButton_recclear_clicked()
{
    ui->lineEdit_recpid->clear();
    ui->lineEdit_rectid->clear();
    ui->spinBox_recduration->setValue(10);
    ui->checkBox_recsw->setCheckState(Qt::CheckState::Unchecked);
    ui->lineEdit_recevent->clear();
    ui->lineEdit_reccpu->clear();
    ui->lineEdit_recfilename->clear();
    ui->lineEdit_recfreq->clear();
    ui->lineEdit_reccallgraph->clear();
}

void MainWindow::on_comboBox_completeregular_currentIndexChanged(const int &arg1)
{
    int offset = 1;
    cmd_completer->setFilterMode(Qt::MatchFlags(arg1+offset));
//    cout << cmd_completer->filterMode();
}

void MainWindow::onTextFilter()
{
//    cout<<lineEdit_filter->text();

    QString keyWord = lineEdit_filter->text();
    QTextDocument *document = ui->textEdit->document();
    if(isHightlighted) document->undo();
    isHightlighted = m_doc.searchHightlight(keyWord,document);
}

void MainWindow::on_pushButton_loadctssuite_clicked()
{
    m_ctsSuite = m_doc.selectDirectory("");
    ui->lineEdit_ctssuite->setText(m_ctsSuite);
    emit sig_sendToXts(m_ctsSuite);
}

void MainWindow::on_comboBox_ctscommand_currentTextChanged(const QString &arg1)
{
    cout<<arg1;
    if(arg1 == "list modules" || arg1 == "list results"){
        ui->comboBox_ctsmodule->setCurrentIndex(1);
        ui->comboBox_ctsmodule->setEnabled(false);
        ui->lineEdit_ctstest->clear();
        ui->lineEdit_ctstest->setEnabled(false);
    }else{
        ui->comboBox_ctsmodule->setEnabled(true);
        ui->lineEdit_ctstest->setEnabled(true);
    }
}

void MainWindow::on_pushButton_runcts_clicked()
{
    QString m_ctsSuite      = ui->lineEdit_ctssuite->text();
    QString m_ctsCommand    = ui->comboBox_ctscommand->currentText();
    QString m_ctsModule     = ui->comboBox_ctsmodule->currentText();
    QString m_ctsTest       = ui->lineEdit_ctstest->text();
    if(m_ctsSuite.isEmpty()){
        QMessageBox::warning(this,"Warning","please select cts suite!");
        return;
    }
    xts->runCts(m_ctsSuite,m_ctsCommand,m_ctsModule,m_ctsTest);
}

void MainWindow::on_pushButton_result_clicked()
{
    cout;
    QString res = m_doc.openFile("*.xml");
    if(!res.isEmpty()){
        QFile file(res);
        if(!file.exists()){
            QMessageBox::warning(this,"warning",QString("file not found!(%1)").arg(res));
            return;
        }
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"warning",QString("file open failed!(%1)").arg(res));
            return;
        }
        ui->textEdit->append("Open Result : "+res);
        if(fileOperation->readXml(&file))
        {
            mtv->setData(fileOperation->m_testResult,fileOperation->m_totalTests,fileOperation->m_pass);
        }
        file.close();
    }
}

void MainWindow::on_pushButton_log_clicked()
{
    if (ui->lineEdit_ctssuite->text().isEmpty())
    {
        QMessageBox::warning(this,"warning","please choose cts suite ...");
        return;
    }
    QString path = ui->lineEdit_ctssuite->text()+"/logs/latest/";
    if(!QDesktopServices::openUrl(QUrl(path))){
        QMessageBox::warning(this,"warning",QString("open folder failed#{%1}").arg(path));
    }
}

void MainWindow::slo_showCtsResult()
{
#if 0
    QDesktopServices::openUrl(QUrl(ui->comboBox_ctssuite->currentText()+"../../../results/latest/test_result.html"));
#endif
    int index = xts->m_ctsSuite.lastIndexOf("/");
    QString res = xts->m_ctsSuite.left(index) + "/../results/latest/test_result.xml";
    cout << res;
    QFile file(res);
    if(!file.exists()){
        QMessageBox::warning(this,"warning",QString("file not found!(%1)").arg(res));
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"warning",QString("file open failed!(%1)").arg(res));
        return;
    }
    ui->textEdit->append("Open Result : "+res);
    if(fileOperation->readXml(&file)){
        mtv->setData(fileOperation->m_testResult,fileOperation->m_totalTests,fileOperation->m_pass);
    }

    file.close();
}

void MainWindow::loadCtsResulotion()
{
    QString csv = CTSRESULOTION;
    QFile file(csv);
    if(!file.exists()){
        QMessageBox::warning(this,"warning",QString("file not found!(%1)").arg(csv));
        return;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"warning",QString("file open failed!(%1)").arg(csv));
        return;
    }
    if(!fileOperation->readCsv(&file)){
        ui->textEdit->append("cts resulotion load fail ...");
    }
    file.close();
}

bool MainWindow::getStatParams()
{
    cout;
    m_statParams = {
        {"app",""},
        {"pid",""},
        {"tid",""},
        {"systemwide",""},
        {"interval",""},
        {"event",""},
        {"duration"," --duration 10"},
        {"sort",""},
        {"cpu",""},
    };
    bool flag=false;
    if(!ui->lineEdit_statapp->text().isEmpty()) {
        m_statParams["app"] = " --app " + ui->lineEdit_statapp->text();
        flag = true;
    }
    if(!ui->lineEdit_statpid->text().isEmpty()){
        m_statParams["pid"] = " -p " + ui->lineEdit_statpid->text();
        flag = true;
    }
    if(!ui->lineEdit_stattid->text().isEmpty()){
        m_statParams["tid"] = " -t " + ui->lineEdit_stattid->text();
        flag = true;
    }
    if(ui->checkBox_statsw->checkState()==Qt::Checked) {
        m_statParams["systemwide"] = " -a ";
        flag = true;
    }
    if(!ui->lineEdit_statinterval->text().isEmpty()) {
        m_statParams["interval"] = " --interval " + ui->lineEdit_statinterval->text();
        flag = true;
    }
    if(!ui->lineEdit_statevent->text().isEmpty()) {
        m_statParams["event"] = " -e " + ui->lineEdit_statevent->text();
        flag = true;
    }
    if(ui->spinBox_statduration->value()!=10 &&
       ui->spinBox_statduration->value()!=0)
    {
        m_statParams["duration"] = " --duration " + ui->spinBox_statduration->text();
        flag = true;
    }
    if(!ui->lineEdit_statsort->text().isEmpty()) {
        m_statParams["sort"] = " --sort " + ui->lineEdit_statsort->text();
        flag = true;
    }
    if(!ui->lineEdit_statcpu->text().isEmpty()) {
        m_statParams["cpu"] = " --cpu " + ui->lineEdit_statcpu->text();
        flag = true;
    }
    return flag;
}

bool MainWindow::getRecordParams()
{
    cout;
    m_recordParams = {
        {"app",""},
        {"pid",""},
        {"tid",""},
        {"systemwide",""},
        {"event",""},
        {"group",""},
        {"toc",""},
        {"callgraph",""},
        {"cpu",""},
        {"frequent",""},
        {"duration"," --duration 10"},
        {"filename"," -o perf.data"},
    };
    bool flag=false;
    if(!ui->lineEdit_recapp->text().isEmpty()){
        m_recordParams["app"] = " --app " + ui->lineEdit_recapp->text();
        flag = true;
    }
    if(!ui->lineEdit_recpid->text().isEmpty()){
        m_recordParams["pid"] = " -p " + ui->lineEdit_recpid->text();
        flag = true;
    }
    if(!ui->lineEdit_rectid->text().isEmpty()){
        m_recordParams["tid"] = " -t " + ui->lineEdit_rectid->text();
        flag = true;
    }
    if(ui->checkBox_recsw->checkState()==Qt::Checked) {
        m_recordParams["systemwide"] = " -a ";
        flag = true;
    }
    if(!ui->lineEdit_recevent->text().isEmpty()) {
        m_recordParams["event"] = " -e " + ui->lineEdit_recevent->text();
        flag = true;
    }
    if(!ui->lineEdit_recgroup->text().isEmpty()) {
        m_recordParams["group"] = " --group " + ui->lineEdit_recgroup->text();
        flag = true;
    }
    if(!ui->lineEdit_rectoc->text().isEmpty()) {
        m_recordParams["toc"] = " --trace-offcpu " + ui->lineEdit_rectoc->text();
        flag = true;
    }
    if(!ui->lineEdit_reccallgraph->text().isEmpty()) {
        m_recordParams["callgraph"] = " --call-graph " + ui->lineEdit_reccallgraph->text();
        flag = true;
    }
    if(!ui->lineEdit_reccpu->text().isEmpty()) {
        m_recordParams["cpu"] =  " --cpu " + ui->lineEdit_reccpu->text();
        flag = true;
    }
    if(!ui->lineEdit_recfreq->text().isEmpty()) {
        m_recordParams["frequent"] = " -f " + ui->lineEdit_recfreq->text();
        flag = true;
    }
    if(ui->spinBox_recduration->value()!=10 &&
       ui->spinBox_recduration->value()!=0)
    {
        m_recordParams["duration"] = " --duration " + ui->spinBox_recduration->text();
        flag = true;
    }
    if(!ui->lineEdit_recfilename->text().isEmpty()) {
        m_recordParams["filename"] = " -o " + ui->lineEdit_recfilename->text();
        flag = true;
    }
    return flag;
}

#if 0
void MainWindow::readfile()
{
    cout;
    QFile file("/home/zuozhe/share/2022/One Click/one_click_generation/cts_test_list.txt");
    QTextStream in(&file);
    if(!file.exists()){
        QMessageBox::warning(NULL,"warning",QString("file:%1 does not exist!")
                             .arg(file.fileName()));
    }
    if(file.open(QIODevice::ReadOnly)){

        while(!in.atEnd()){
            cout << in.readLine();
            if(in.readLine().contains("[1]"))
            {
                cout << in.readLine().data();
            }
        }
    }
}
#endif



void MainWindow::on_pushButton_screencapture_clicked()
{
    QString img = QString("/storage/img_%1%2").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")).arg(".png");
    cout << img;
    QString path = "capture";
    QDir dir(path);
    if(!dir.exists()) dir.mkpath(path);
    ccd->runCommand(SCREENCAPTURE+img+QString(";adb pull %1 %2").arg(img).arg(path));
}

void MainWindow::on_pushButton_screenrecord_clicked()
{
    cout ;
//    QString record = QString("/storage/record_%1%2").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")).arg(".mp4");
//    ccd->runCommand(SCREENRECORD+record+QString(";adb pull %1 .").arg(record));
}



void MainWindow::on_pushButton_fastboot_clicked()
{
    ccd->runCommand(FASTBOOT);
}

void MainWindow::on_pushButton_clearlogcat_clicked()
{
    ccd->runCommand(CLEARLOGCAT);
}

void MainWindow::on_pushButton_adbrestart_clicked()
{
    ccd->runCommand(ADBRESTART);
}

void MainWindow::on_pushButton_sentest_clicked()
{
    ccd->runCommand(SENTEST);
}

void MainWindow::on_pushButton_killcamera_clicked()
{
    ccd->runCommand(KILLCAMERASERVER);
}

void MainWindow::on_pushButton_dumpcamera_clicked()
{
    ccd->runCommand(DUMPCAMERA);
}

void MainWindow::on_pushButton_property_clicked()
{
    ccd->runCommand(PROPERTY);
}

void MainWindow::on_pushButton_drawid_clicked()
{
    ccd->runCommand(DRAWID);
}

void MainWindow::on_pushButton_opencamera_clicked()
{
    ccd->runCommand(OPENCAMERA);
}

void MainWindow::on_pushButton_takepicture_clicked()
{
    ccd->runCommand(TAKEPICTURE);
}

void MainWindow::on_pushButton_screensize_clicked()
{
    ccd->runCommand(SCREENSIZE);
}

void MainWindow::on_pushButton_cpuinfo_clicked()
{
    ccd->runCommand(CPUINFO);
}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->textEdit->clear();
}

void MainWindow::on_pushButton_batterystats_clicked()
{
    batterystats->show();
}



