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
    QString     m_hostName;
    QString     m_totalName;

    Simpleperf *simpleperf = new Simpleperf();
    QCompleter *completer;
    FileOperation *fileOperation = new FileOperation(this);

private:
    Ui::MainWindow *ui;

protected:
    QString     m_msg;
    QStringList m_nativeCmdList;
    QStringList m_customCmdList;
    QString     m_statusbarMsg;
    Document    m_doc;
    void initUi();
    void initEnvironment();
    void initConnect();
    void keyPressEvent(QKeyEvent *event);

signals:


private slots:
    void slotReciveDocument();
    void slotReciveSimpleperf(QString msg);
    void slotReciveSimpleperf(QProcess::ProcessState newState);
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
