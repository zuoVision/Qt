#ifndef DIALOGBATTERYSTATS_H
#define DIALOGBATTERYSTATS_H

#include <QDialog>

namespace Ui {
class DialogBatterystats;
}

class DialogBatterystats : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBatterystats(QWidget *parent = nullptr);
    ~DialogBatterystats();

private:
    void init();
    void initConnect();

private:
    Ui::DialogBatterystats *ui;

signals:
    void sig_batterystat(QString cmd);

private slots:
    void on_pushButton_reset_clicked();
    void on_pushButton_open_clicked();
    void on_pushButton_open_2_clicked();
};

#endif // DIALOGBATTERYSTATS_H
