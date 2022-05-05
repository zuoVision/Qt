#include "dialogbatterystats.h"
#include "ui_dialogbatterystats.h"
#include "cmd.h"
#include "document.h"

#include <QDebug>
#include <QMessageBox>
#include <QThread>
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
    this->setFixedSize(QSize(400,300));
    ui->label_batteryhistorianurl->setText(BATTERYHISTORIANURL);
    ui->label_batteryhistorianurl->setToolTip(QString("open with browser"));
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
    mState = QProcess::ProcessState::NotRunning;
    mCurrentStatus = STATUS::stop;
}

/**
 * @brief DialogBatterystats::onStart
 */
void DialogBatterystats::onStart()
{
    cout;
    mCurrentStatus = STATUS::start;
    if(isNeedReset()){
        cout << mState;
        if(mState == QProcess::ProcessState::NotRunning){

//            ui->pushButton_dumpdata->setText("start");
            emit sig_batterystat(BATTERYSTATS " --reset");
            cout ;
        }else{
            cout;
            return;
            //TODO:解决应用启动processor处于runing状态，导致reset命令无法执行
            //CommonCommand [ runCommand : 89 ] command is empty or process is running!
        }
    }else{
        onStats();
    }
}

/**
 * @brief DialogBatterystats::onStats
 */
void DialogBatterystats::onStats()
{
    cout;
    mCurrentStatus = STATUS::stats;
    ui->pushButton_dumpdata->setText("stop");
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
    if(timer->isActive()) timer->stop();
    mCurrentStatus = STATUS::stop;
    ui->pushButton_dumpdata->setEnabled(false);
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
    emit sig_batterystat(cmd);
    ui->progressBar_timer->setFormat("dumping...");
    mDumpFlag = true;
}

/**
 * @brief DialogBatterystats::onDestroy
 */
void DialogBatterystats::onDestroy()
{
    ui->progressBar_timer->setValue(0);
    ui->progressBar_timer->setFormat("%p%");
    ui->progressBar_timer->hide();
    if (!ui->pushButton_dumpdata->isEnabled()){
        ui->pushButton_dumpdata->setEnabled(true);
        ui->pushButton_dumpdata->setText("start");
    }
    mState = QProcess::ProcessState::NotRunning;
    mCurrentStatus = STATUS::stop;
    delete timer;
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
    cout<<mResetFlag;
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
    ui->pushButton_dumpdata->setText("stop");
    ui->progressBar_timer->show();
    ui->progressBar_timer->setMaximum(ui->spinBox_time->text().toInt());
    ui->progressBar_timer->setMinimum(0);
    ui->progressBar_timer->setFormat("stats... %p%");
}

/**
 * @brief DialogBatterystats::onManulStats
 */
void DialogBatterystats::onManulStats()
{
    cout;
    ui->pushButton_dumpdata->setText("stop");
    ui->progressBar_timer->show();
    ui->progressBar_timer->setFormat("stats...");

}

void DialogBatterystats::onCount()
{
    cout;
    mCounter++;
    int time = ui->spinBox_time->text().toInt();
    cout << mCounter;
    ui->progressBar_timer->setValue(mCounter);
    if (mCounter>=time && mCurrentStatus==STATUS::stats) {      
        onStop();
    }
}

/**
 * @brief DialogBatterystats::slo_reciveMessage
 * @param result
 */
void DialogBatterystats::slo_reciveMessage(QString result)
{
    cout << result;

    if(result.contains("Battery stats reset")) {
        mResult = true;
    }else if(result.startsWith("Warning")){
        mResult = false;
    }
}

/**
 * @brief DialogBatterystats::slo_reciveMessage
 * @param state
 * @param tag
 */
void DialogBatterystats::slo_reciveMessage(QProcess::ProcessState state, QString tag)
{
    cout << state << tag;
    mState = state;
    if(state == QProcess::ProcessState::NotRunning){
        cout << "ProcessState::NotRunning";
        if(mResetFlag){
            cout << "reset completed!";
            mResetFlag=false;
            if (mResult){
                onStats();
            }else{
                onDestroy();
            }
        }
        if(mDumpFlag){
            cout<<"dump completed!";
            mDumpFlag = false;
            onDestroy();
        }

    }else{
        cout << "ProcessState::Running";
    }
}

/**
 * @brief DialogBatterystats::on_pushButton_open_clicked
 */
void DialogBatterystats::on_pushButton_open_clicked()
{
    cout;
    Document doc;
    QString path = doc.selectDirectory("");
    ui->lineEdit_path->setText(path);
}

/**
 * @brief DialogBatterystats::on_pushButton_dumpdata_clicked
 */
void DialogBatterystats::on_pushButton_dumpdata_clicked()
{
    cout;
    if(mCurrentStatus == STATUS::stop){
        onStart();      
    }else if(mCurrentStatus == STATUS::stats) {
        onStop();
    }
}

/**
 * @brief DialogBatterystats::on_radioButton_auto_toggled
 * @param checked
 */
void DialogBatterystats::on_radioButton_auto_toggled(bool checked)
{
    ui->spinBox_time->setEnabled(checked);
    ui->progressBar_timer->hide();
}
