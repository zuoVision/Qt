#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QLabel>

class MyTableView : public QTableView
{
public:
    MyTableView();
    ~MyTableView();

    void init();
    void setData(QVector<QStringList> * data,QString arg1,QString arg2);

public:
    QStandardItemModel  *model;
    QString             html_format;
    QLabel              *lab_resulotion;
};

#endif // MYTABLEVIEW_H
