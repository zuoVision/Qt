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

#include "document.h"
#include "listenerthread.h"
#include "simpleperf.h"
#include "fileoperation.h"
#include "xts.h"
#include "commoncommand.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow :
        public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString     m_userName;

    CommonCommand   *ccd = new CommonCommand();
    Simpleperf      *simpleperf = new Simpleperf();
    Xts             *xts = new Xts(this);
    QCompleter      *completer;
    FileOperation   *fileOperation = new FileOperation(this);

private:
    Ui::MainWindow *ui;

protected:
    QString                     m_msg;
    QStringList                 m_nativeCmdList;
    QStringList                 m_customCmdList;
    QProcess::ProcessState      m_processState=QProcess::ProcessState::NotRunning;
    QString                     m_statusbarMsg="Process Not Running";
    Document                    m_doc;
    void initUi();
    void initEnvironment();
    void initConnect();
    void keyPressEvent(QKeyEvent *event);

signals:


private slots:
    void slotReciveDocument();
    void slo_reciveMessage(QString msg);
    void slo_reciveMessage(QProcess::ProcessState state);
    void slo_showCtsSuite();
    void moveCursorToEnd();
    void on_pushButton_run_clicked();

    void on_pushButton_devices_clicked();
    void on_pushButton_root_clicked();
    void on_pushButton_remount_clicked();
    void on_pushButton_oemunlock_clicked();

    void on_pushButton_stat_clicked();
    void on_pushButton_record_clicked();
    void on_pushButton_report_clicked();
    void on_pushButton_flamegraph_clicked();
    void on_pushButton_doc_clicked();

    void on_pushButton_cts_clicked();
    void on_comboBox_completeregular_currentIndexChanged(const int &arg1);
};
#endif // MAINWINDOW_H
