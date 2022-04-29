#include "dialogbatterystats.h"
#include "ui_dialogbatterystats.h"
#include "cmd.h"
#include "document.h"

#include <QDebug>
#include <QMessageBox>
#define MY_TAG "DialogBatterystats"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"


DialogBatterystats::DialogBatterystats(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBatterystats)
{
    ui->setupUi(this);
    init();
    onCreate();
    initConnect();

}

DialogBatterystats::~DialogBatterystats()
{
    delete ui;
}

/**
 * @brief DialogBatterystats::init
 */
void DialogBatterystats::init()
{
    ui->label_batteryhistorianurl->setText(BATTERYHISTORIANURL);
    ui->label_batteryhistorianurl->setOpenExternalLinks(true);
    ui->progressBar_timer->hide();
}

/**
 * @brief DialogBatterystats::initConnect
 */
void DialogBatterystats::initConnect()
{
    connect(timer,SIGNAL(timeout()),
            this,SLOT(onCount()));
}

/**
 * @brief DialogBatterystats::onCreate
 */
void DialogBatterystats::onCreate()
{
    cout;
    //timer
    timer = new QTimer();
    currentStatus = STATUS::stopStats;
}

/**
 * @brief DialogBatterystats::onStart
 */
void DialogBatterystats::onStart()
{
    cout;
    if(isNeedReset()){
        emit sig_batterystat(BATTERYSTATS " --reset");
    }else{
        selectMode();
    }

//    currentStatus = STATUS::startStats;
//    ui->pushButton_dumpdata->setText("stop");

//    //timer count
//    if(ui->radioButton_auto->isChecked()){
//        timer->start();
//        timer->setInterval(1000);
//        mCounter = 0;
//        ui->progressBar_timer->show();
//        ui->progressBar_timer->setMaximum(ui->spinBox_time->text().toInt());
//        ui->progressBar_timer->setMinimum(0);
//        ui->progressBar_timer->setFormat("stats...%p%");
//        ui->pushButton_dumpdata->setEnabled(false);
//    }else {
//        cout << " Please stop manually! ";
//    }
}

/**
 * @brief DialogBatterystats::onStats
 */
void DialogBatterystats::onStats()
{
    cout;
    mStatsFlag = true;
    if(isAutoMode()){
        onAutoStats();
    }else{
        onManulStats();
    }
}

/**
 * @brief DialogBatterystats::onStop
 *
 */
void DialogBatterystats::onStop()
{
    cout;
    onDump();
}

/**
 * @brief DialogBatterystats::onDump
 */
void DialogBatterystats::onDump()
{
    cout;
    QString cmd = BATTERYSTATS;
    if(ui->lineEdit_path->text().isEmpty()){
        cmd += QString("%1 %2%3")
                .arg(" > batterystats.txt;")
                .arg(BUGREPORT)
                .arg(" bugreport.zip");
    }else{
        cmd += QString("%1 %2%3 %4 %5%6")
                .arg(" > ")
                .arg(ui->lineEdit_path->text())
                .arg("/batterystats.txt;")
                .arg(BUGREPORT)
                .arg(ui->lineEdit_path->text())
                .arg("/bugreport.zip");
    }

    cout << cmd;
    emit sig_batterystat("pwd");
    mDumpFlag = true;
}

void DialogBatterystats::onDestroy()
{
    cout;
}

/**
 * @brief DialogBatterystats::isNeedReset
 * @return bool
 */
bool DialogBatterystats::isNeedReset()
{
    cout;
    setResetFlag(ui->checkBox_reset->isEnabled());
    if(mResetFlag){
        return true;
    }else{
        return false;
    }
}

/**
 * @brief DialogBatterystats::setResetFlag
 * @param flag
 */
void DialogBatterystats::setResetFlag(bool flag)
{
    cout;
    mResetFlag = flag;
}

/**
 * @brief DialogBatterystats::selectMode
 */
void DialogBatterystats::selectMode()
{
    cout;
}

/**
 * @brief DialogBatterystats::isAutoMode
 * @return
 */
bool DialogBatterystats::isAutoMode()
{
    cout;
    if(ui->radioButton_auto->isChecked()) return true;
    return false;
}

/**
 * @brief DialogBatterystats::isManulMode
 * @return
 */
bool DialogBatterystats::isManulMode()
{
    cout;
    if(ui->radioButton_manul->isChecked()) return true;
    return false;
}

/**
 * @brief DialogBatterystats::onAutoStats
 */
void DialogBatterystats::onAutoStats()
{
    cout;
    mCounter = 0;
    timer->setInterval(1000);
    timer->start();
}

/**
 * @brief DialogBatterystats::onManulStats
 */
void DialogBatterystats::onManulStats()
{
    cout;
    onStats();
}



void DialogBatterystats::onCount()
{
    cout;
    mCounter++;
    int time = ui->spinBox_time->text().toInt();
    cout << mCounter;
    ui->progressBar_timer->setValue(mCounter);
    if (mCounter>=time && mStatsFlag) {
        mStatsFlag = false;
        timer->stop();
        onStop();
//        ui->progressBar_timer->setFormat("dump data...");
//        ui->pushButton_dumpdata->setEnabled(false);
//        timer->stop();
//        currentStatus = STATUS::stopStats;
//        onProcess();
    }
}

void DialogBatterystats::onProcess()
{
    cout;
}

void DialogBatterystats::slo_reciveMessage(QProcess::ProcessState state, QString tag)
{
    cout << state << tag;
    if(state == QProcess::ProcessState::NotRunning){
        if(mResetFlag){
            cout << "reset completed!";
            mResetFlag=false;
            onStats();
        }
        if(mDumpFlag){
            cout<<"dump completed!";
            mDumpFlag = false;
            onDestroy();
        }
        if (!ui->pushButton_dumpdata->isEnabled()){
            ui->pushButton_dumpdata->setEnabled(true);
            ui->pushButton_dumpdata->setText("start");
        }

    }else{
//        ui->pushButton_dumpdata->setText("stop");
//        ui->pushButton_dumpdata->setEnabled(true);
//        currentStatus = STATUS::stopStats;
    }
}

void DialogBatterystats::on_pushButton_open_clicked()
{
    cout;
    Document doc;
    QString path = doc.selectDirectory("");
    ui->lineEdit_path->setText(path);
}

void DialogBatterystats::on_pushButton_dumpdata_clicked()
{
    cout;
    if(currentStatus == STATUS::stopStats){
        onStart();      
    }else {
        onStop();
    }
}

void DialogBatterystats::on_radioButton_auto_toggled(bool checked)
{
    ui->spinBox_time->setEnabled(checked);
}
