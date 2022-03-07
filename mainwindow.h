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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private:
    Ui::MainWindow *ui;

    QProcess *p = new QProcess(this);
    void excuteCmd(QStringList params);
    void excuteCmd(QString filePath, QStringList params);
    QStringList cmd;

private slots:

    void on_readoutput();
    void on_readerror();

    void on_pushButton_run_clicked();

    void on_pushButton_devices_clicked();
    void on_pushButton_root_clicked();
    void on_pushButton_remount_clicked();
    void on_pushButton_oemunlock_clicked();

    void on_pushButton_stat_clicked();
    void on_pushButton_record_clicked();
    void on_pushButton_report_clicked();
    void on_pushButton_flamegraph_clicked();
};
#endif // MAINWINDOW_H
