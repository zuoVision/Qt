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
    m_ssh_status = new QLabel("ssh",this);
    m_ccd_status->setFixedSize(20,20);
    m_sim_status->setFixedSize(20,20);
    m_xts_status->setFixedSize(20,20);
    m_ssh_status->setFixedSize(20,20);
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
    m_ssh_status->setScaledContents(true);
    m_ccd_status->setPixmap(*led_grey);
    m_sim_status->setPixmap(*led_grey);
    m_xts_status->setPixmap(*led_grey);
    m_ssh_status->setPixmap(*led_grey);
    ui->statusbar->addWidget(m_ccd_status);
    ui->statusbar->addWidget(m_sim_status);
    ui->statusbar->addWidget(m_xts_status);
    ui->statusbar->addWidget(m_ssh_status);

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
    loadProjectInfo();
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

    //SSH
    connect(ssh,SIGNAL(onSubmitOutput(QString)),
            this,SLOT(onReciveOutput(QString)));
    connect(ssh,SIGNAL(onSubmitError(QString)),
            this,SLOT(onReciveError(QString)));
    connect(ssh,SIGNAL(onSubmitInfo(QString)),
            this,SLOT(onReciveInfo(QString)));
    connect(ssh,SIGNAL(onSubmitState(int,QProcess::ProcessState)),
            this,SLOT(onReciveState(int,QProcess::ProcessState)));
    connect(ssh,SIGNAL(onSubmitExitStatus(int,QProcess::ExitStatus)),
            this,SLOT(onReciveExitStatus(int,QProcess::ExitStatus)));

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
    connect(this,SIGNAL(sig_sendToBatterystats(QString)),
            batterystats,SLOT(slo_reciveMessage(QString)));
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
        emit sig_sendToBatterystats(msg);
        return;
    }
    ui->textEdit->append(color.BLACK.arg(msg));
    emit sig_sendToBatterystats(msg);
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
            if(!ui->lineEdit_cmd->text().isEmpty()) ui->lineEdit_cmd->clear();
            emit sig_sendToBatterystats(state,tag);
        }
        if (tag == "Simpleperf") m_sim_status->setPixmap(*led_grey);
        if (tag == "Xts") m_xts_status->setPixmap(*led_grey);
//            m_statusbarMsg = "    Process Not Running ";

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

/**
 * @brief MainWindow::onReciveOutput
 * @param output
 */
void MainWindow::onReciveOutput(QString output)
{
    cout << output;
    ui->textEdit->append(color.BLUE.arg(output));
}

/**
 * @brief MainWindow::onReciveError
 * @param error
 */
void MainWindow::onReciveError(QString error)
{
   cout << error;
   ui->textEdit->append(color.RED.arg(error));
}

/**
 * @brief MainWindow::onReciveInfo
 * @param info
 */
void MainWindow::onReciveInfo(QString info)
{
   cout << info;
   ui->textEdit->append(color.BLACK.arg(info));
}

/**
 * @brief MainWindow::onReciveState
 * @param state
 */
void MainWindow::onReciveState(int tag,QProcess::ProcessState state)
{
    cout << state;
    switch (tag) {
         case COMMAND:
             cout << COMMAND;
             break;
         case SIMPLEPERF:
             cout << SIMPLEPERF;
             break;
         case XTS:
             cout << XTS;
             break;
         case SSH:
             cout << SSH;
             if(state == ProcessState::Starting)    m_ssh_status->setPixmap(*led_blue);
             if(state == ProcessState::Running)     m_ssh_status->setPixmap(*led_green);
             if(state == ProcessState::NotRunning)  m_ssh_status->setPixmap(*led_grey);
             break;
         default:
             break;
    }
}

/**
 * @brief MainWindow::onReciveExitStatus
 * @param exitStatus
 */
void MainWindow::onReciveExitStatus(int tag,QProcess::ExitStatus exitStatus)
{
   cout << tag << exitStatus;
   switch (tag) {
        case COMMAND:
            cout << COMMAND;
            break;
        case SIMPLEPERF:
            cout << SIMPLEPERF;
            break;
        case XTS:
            cout << XTS;
            break;
        case SSH:
            cout << SSH;
            break;
        default:
            break;
   }
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

/**
 * @brief MainWindow::loadProjectInfo
 */
void MainWindow::loadProjectInfo()
{
    cout << PROJECTFILE;

    mProjectInfo = mParseXml.parseProjectXml(PROJECTFILE);
    if (mProjectInfo.isEmpty()) {
        cout << "项目为空！";
    }else{
        ui->comboBox_project->clear();
        for(QVector<PROJECT>::iterator it = mProjectInfo.begin();
            it!=mProjectInfo.end();it++){
            ui->comboBox_project->addItem(it->projectName);
        }
    }
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

/**
 * @brief MainWindow::callback
 * @param state
 */
void MainWindow::callback(CallbackState state)
{
    switch (state) {
    case CallbackState::Success:
        cout << CallbackState::Success;
        break;
    case CallbackState::Error:
        cout << CallbackState::Error;
        break;
    case CallbackState::Unknow:
        cout << CallbackState::Unknow;
        break;
    default:
        cout << CallbackState::Success;
        break;
    }

}

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

void MainWindow::on_pushButton_login_clicked()
{
    QString CMD = QString("ssh %1 'ls -d */' > %2");
    ssh->login(CMD.arg(ui->lineEdit_ssh->text()).arg(SSHROOTDIR),callback);

    QFile file(SSHROOTDIR);
    ui->comboBox_localproject->addItems(fileOperation->readTxt(&file));
    ui->lineEdit_ssh->setEnabled(false);
    ui->pushButton_login->setEnabled(false);
    ui->pushButton_logout->setEnabled(true);
    ui->pushButton_browse->setEnabled(true);
    ui->pushButton_download->setEnabled(true);
    ui->pushButton_build->setEnabled(true);
}

void MainWindow::on_pushButton_logout_clicked()
{
    ssh->logout();
    ui->comboBox_localproject->clear();
    ui->comboBox_buildversion->clear();
    ui->textEdit->append(color.BLACK.arg(QString("Connection to %1 closed.").arg(ui->lineEdit_ssh->text())));
    ui->lineEdit_ssh->setEnabled(true);
    ui->pushButton_login->setEnabled(true);
    ui->pushButton_logout->setEnabled(false);
    ui->pushButton_browse->setEnabled(false);
    ui->pushButton_download->setEnabled(false);
    ui->pushButton_build->setEnabled(false);
}

void MainWindow::on_comboBox_project_currentTextChanged(const QString &arg1)
{
//    cout << arg1;
    QStringList branchList;
    branchList = mParseXml.searchBranch(arg1,&mProjectInfo);
    if(!branchList.isEmpty()){
        ui->comboBox_branch->clear();
        ui->comboBox_branch->addItems(branchList);
    }
}

void MainWindow::on_pushButton_download_clicked()
{
//    cout << ui->comboBox_project->currentText() << ui->comboBox_branch->currentText();
    if(ui->comboBox_project->currentText().isEmpty() ||
       ui->comboBox_branch->currentText().isEmpty()){
        QMessageBox::warning(this,"warning","please select project & branch!");
        return;
    }
    QString downloadCmd = QString("ssh %1 'mkdir %2 && cd %2 && %3 && %4 -j %5'");
    QString repoCmd;
    repoCmd = mParseXml.searchRepo(ui->comboBox_project->currentText(),
                                  ui->comboBox_branch->currentText(),
                                  &mProjectInfo);
    if(!repoCmd.isEmpty()){
        ssh->run(downloadCmd.arg(ui->lineEdit_ssh->text())
                            .arg(ui->comboBox_project->currentText())
                            .arg(repoCmd)
                            .arg(REPOSYNC)
                            .arg(ui->spinBox_j->text()));
    }
}

/**
 * @brief MainWindow::on_comboBox_localproject_currentTextChanged
 * @param arg1
 */
void MainWindow::on_comboBox_localproject_currentTextChanged(const QString &arg1)
{
//    cout << arg1;
    ui->comboBox_buildversion->clear();
    QStringList buildVer;
    buildVer = mParseXml.searchBuildVer(arg1,&mProjectInfo);
    if(!buildVer.isEmpty()){        
        ui->comboBox_buildversion->addItems(buildVer);
    }
}

/**
 * @brief MainWindow::on_comboBox_buildversion_currentTextChanged
 * @param arg1
 */
void MainWindow::on_comboBox_buildversion_currentTextChanged(const QString &arg1)
{
    if(arg1 == "ninja"){
        ui->lineEdit_so->setEnabled(true);
    }else{
        ui->lineEdit_so->setEnabled(false);
    }
}

/**
 * @brief MainWindow::on_pushButton_build_clicked
 */
void MainWindow::on_pushButton_build_clicked()
{
    cout;
    QString CMD = QString("ssh %1 'cd %2 && %3 -j %4'");
    QString buildCmd;
    if(ui->comboBox_buildversion->currentText() == "ninja"){
        buildCmd = NINJA "out/"+mNinjaFile+"/"+ui->lineEdit_so->text();
    }else{
        buildCmd = mParseXml.searchBuildCmd(ui->comboBox_localproject->currentText(),
                                                ui->comboBox_buildversion->currentText(),
                                                &mProjectInfo);
    }
    if(!buildCmd.isEmpty()){
        ssh->run(CMD.arg(ui->lineEdit_ssh->text())
                    .arg(ui->comboBox_localproject->currentText())
                    .arg(buildCmd)
                    .arg(ui->spinBox_j->text()));
    }
}

/**
 * @brief MainWindow::on_pushButton_browse_clicked
 */
void MainWindow::on_pushButton_browse_clicked()
{
    QString CMD = QString("ssh -X %1 nautilus");
    ssh->run(CMD.arg(ui->lineEdit_ssh->text()));
}
