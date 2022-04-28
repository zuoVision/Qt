#ifndef DIALOGBATTERYSTATS_H
#define DIALOGBATTERYSTATS_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class DialogBatterystats;
}

enum STATUS{
    startStats,
    stopStats,
};

class DialogBatterystats : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBatterystats(QWidget *parent = nullptr);
    ~DialogBatterystats();

private:
    void init();
    void initConnect();
    void onStart();
    void onStop();

private:
    QTimer  *timer;
    bool    isTimerEnable = false;

    STATUS currentStatus;

private:
    Ui::DialogBatterystats *ui;

signals:
    void sig_batterystat(QString cmd);

private slots:
    void onTimeEnable(bool checked);
    void onProcess();

    void on_pushButton_open_clicked();
    void on_pushButton_dumpdata_clicked();
};

#endif // DIALOGBATTERYSTATS_H
