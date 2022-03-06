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

//    QProcess *p;
    QProcess *p = new QProcess(this);
    void init_process();
    void excuteCmd(QString filePath);
    void excuteCmd(QString filePath, QStringList params);

private slots:
    void on_pushButton_run_clicked();
    void on_pushButton_stat_clicked();
    void on_pushButton_record_clicked();
    void on_pushButton_flamegraph_clicked();

    void on_readoutput();
    void on_readerror();
};
#endif // MAINWINDOW_H
