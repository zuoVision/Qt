#ifndef DIALOGBATTERYSTATS_H
#define DIALOGBATTERYSTATS_H

#include <QDialog>
#include <QProcess>
#include <QTimer>

namespace Ui {
class DialogBatterystats;
}

enum STATUS{
    start,
    stats,
    stop,
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
    void onCreate();
    void onStart();
    void onStats();
    void onStop();
    void onDump();
    void onDestroy();

    bool isNeedReset();
    void setResetFlag(bool flag);

    void selectMode();
    bool isAutoMode();
    bool isManulMode();


    void onAutoStats();
    void onManulStats();

private:
    bool                    mResetFlag=false;
    bool                    mStatsFlag=false;
    bool                    mDumpFlag=false;
    bool                    mResult=false;

    QTimer                  *timer;
    int                     mCounter;
    QProcess::ProcessState  mState;
    STATUS                  mCurrentStatus;


private:
    Ui::DialogBatterystats *ui;

signals:
    void sig_batterystat(QString cmd);

private slots:
    void onCount();
    void slo_reciveMessage(QString result);
    void slo_reciveMessage(QProcess::ProcessState state,QString tag);

    void on_pushButton_open_clicked();
    void on_pushButton_dumpdata_clicked();
    void on_radioButton_auto_toggled(bool checked);
};

#endif // DIALOGBATTERYSTATS_H
