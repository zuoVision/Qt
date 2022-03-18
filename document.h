#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>

namespace Ui {
class Document;
}

class Document : public QWidget
{
    Q_OBJECT

public:
    explicit Document(QWidget *parent = nullptr);
    ~Document();
    void init();

    QString filePath;
    void onLoadDocument(QString doc);

private:
    Ui::Document *ui;

private slots:
    void on_pushButton_open_clicked();
    void on_pushButton_save_clicked();
};

#endif // DOCUMENT_H
