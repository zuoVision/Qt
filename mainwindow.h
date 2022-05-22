#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qdesktopservices.h>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QLineEdit>
#include <QByteArray>
#include <QMessageBox>
#include <QFile>
#include <QThread>
#include <QThreadPool>
#include <QCompleter>
#include <QTableWidget>

#include <utils/parseXml/parseXml.h>

#include "utils/general/general.h"
#include "document.h"
#include "listenerthread.h"
#include "simpleperf.h"
#include "fileoperation.h"
#include "xts.h"
#include "commoncommand.h"
#include "mytableview.h"
#include "dialogbatterystats.h"
#include "ssh.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; class DialogBatterystats; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    typedef QProcess::ProcessState ProcessState;
    typedef QVector<PROJECT> ProjectInfo;

//    QString         m_userName;

    DialogBatterystats *batterystats = new DialogBatterystats(this);
    CommonCommand   *ccd = new CommonCommand();
    Simpleperf      *simpleperf = new Simpleperf();
    Xts             *xts = new Xts(this);
    Ssh             *ssh = new Ssh();

    QCompleter      *cmd_completer;
    QCompleter      *test_completer;
    FileOperation   *fileOperation = new FileOperation(this);
    MyTableView     *mtv = new MyTableView();
    QLineEdit       *lineEdit_filter;
    bool            isHightlighted = false;

public:
    QString         m_ctsSuite;
    QStringList     m_resolution;

    QLabel          *m_ccd_status;
    QLabel          *m_sim_status;
    QLabel          *m_xts_status;
    QLabel          *m_ssh_status;
    QPixmap         *led_red;
    QPixmap         *led_green;
    QPixmap         *led_yellow;
    QPixmap         *led_grey;
    QPixmap         *led_blue;
    COLOR           color;

private:
    Ui::MainWindow  *ui;

protected:
    QString                     m_msg;
    QStringList                 m_nativeCmdList;
    QStringList                 m_customCmdList;
    QStringList                 m_ctsTestList;
    QProcess::ProcessState      m_processState=QProcess::ProcessState::NotRunning;
    QString                     m_statusbarMsg="Process Not Running";
    Document                    m_doc;

    //simpleperf
    std::map<QString,QString>   m_statParams;
    std::map<QString,QString>   m_recordParams;

    //ssh
    ProjectInfo                 mProjectInfo;
    BRANCH                      mBranch;
    REPO                        mRepo;
    BUILD                       mBuild;
    ParseXml                    mParseXml;
    QString                     mNinjaFile;
    METADATA*                   mMetadata;

protected:
    void initUi();
    void initEnvironment();
    void initConnect();
    void keyPressEvent(QKeyEvent *event);
//    void resizeEvent(QResizeEvent *event);
    void onContrlSearchBar();
    void loadCtsResulotion();
    void loadProjectInfo();
    void insertDataToTable();
    void readfile();
    bool getStatParams();
    bool getRecordParams();

private:
    void loginSuccess();
    void logoutSuccess();

signals:
    void sig_sendToXts(QString);
    void sig_sendToBatterystats(QString);
    void sig_sendToBatterystats(QProcess::ProcessState,QString);

private slots:
    void slotReciveDocument();
    void slo_batterystats(QString cmd);
    void slo_reciveMessage(QString msg);
    void slo_reciveMessage(QProcess::ProcessState state,QString tag);

    void slo_openDocument();
    void moveCursorToEnd();
    void on_comboBox_completeregular_currentIndexChanged(const int &arg1);
    void onTextFilter();

    void onReciveOutput(QString output);
    void onReciveError(QString error);
    void onReciveInfo(QString info);
    void onReciveState(int tag,QProcess::ProcessState state);
    void onReciveExitStatus(int tag,QProcess::ExitStatus exitStatus);
    void onReciveMetadata(METADATA* metadata);

    void onShowCtsResult();

    //command
    void on_pushButton_run_clicked();
    void on_pushButton_devices_clicked();
    void on_pushButton_root_clicked();
    void on_pushButton_remount_clicked();
    void on_pushButton_oemunlock_clicked();
    void on_pushButton_screencapture_clicked();
    void on_pushButton_screenrecord_clicked();
    void on_pushButton_fastboot_clicked();
    void on_pushButton_clearlogcat_clicked();
    void on_pushButton_adbrestart_clicked();
    void on_pushButton_sentest_clicked();
    void on_pushButton_killcamera_clicked();
    void on_pushButton_dumpcamera_clicked();
    void on_pushButton_property_clicked();
    void on_pushButton_drawid_clicked();
    void on_pushButton_cpuinfo_clicked();
    void on_pushButton_opencamera_clicked();
    void on_pushButton_takepicture_clicked();
    void on_pushButton_screensize_clicked();
    void on_pushButton_clear_clicked();
    void on_pushButton_batterystats_clicked();

    //simpleperf
    void on_pushButton_list_clicked();
    void on_pushButton_stat_clicked();
    void on_pushButton_record_clicked();
    void on_pushButton_report_clicked();
    void on_pushButton_flamegraph_clicked();
    void on_pushButton_quickgeneration_clicked();
    void on_pushButton_statclear_clicked();
    void on_pushButton_recclear_clicked();

    //xts
    void on_pushButton_runcts_clicked();
    void on_pushButton_loadctssuite_clicked();
    void on_comboBox_ctscommand_currentTextChanged(const QString &arg1);
    void on_pushButton_result_clicked();
    void on_pushButton_log_clicked();

    //ssh
    void on_pushButton_login_clicked();
    void on_pushButton_logout_clicked();
    void on_comboBox_project_currentTextChanged(const QString &arg1);
    void on_pushButton_download_clicked();
    void on_comboBox_localproject_currentTextChanged(const QString &arg1);
    void on_comboBox_buildversion_currentTextChanged(const QString &arg1);
    void on_pushButton_build_clicked();
    void on_pushButton_browse_clicked();
};
#endif // MAINWINDOW_H
