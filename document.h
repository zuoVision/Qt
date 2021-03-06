#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QWidget>
#include <QCloseEvent>
#include <QTextDocument>

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


    void        onLoadDocument(QString doc);
    QString     openFile(QString filter="");
    QString     selectDirectory(QString dir);
    bool        searchHightlight(QString keyword,QTextDocument *document);

public:
    QString m_filePath;

private:
    Ui::Document *ui;
    void closeEvent(QCloseEvent *);

signals:
    void closed();
private slots:
    void on_pushButton_open_clicked();
    void on_pushButton_save_clicked();
};

#endif // DOCUMENT_H
