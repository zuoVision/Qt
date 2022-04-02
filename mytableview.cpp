#include "mytableview.h"

#include <QDebug>
#include <QLabel>
#include <QTextEdit>

#define MY_TAG "MyTableView"
#define cout    qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]" << __LINE__

MyTableView::MyTableView()
{
    cout;
    model= new QStandardItemModel();
    this->setModel(model);
    init();
}

MyTableView::~MyTableView()
{
    cout;
}

void MyTableView::init()
{
    this->setSortingEnabled(true);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setAutoScroll(true);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    this->setStyleSheet("QHeaderView::section{background:lightgreen;}");
//    model->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightgreen;}");
    html_format="<a href=\"%1\">%2";
    lab_resulotion = new QLabel();
    lab_resulotion->setOpenExternalLinks(true);
    model->setColumnCount(3);
    model->setRowCount(5);
    model->setHorizontalHeaderLabels(QStringList()<<"Test"<<"Result"<<"Resulotion");
//    QString bd = "<a href=\'https://www.baidu.com\'>baidu";
//    QString mtk = "<a href=\'https://online.mediatek.com/\'>MTK";
//    lab_resulotion->setText(bd+mtk);
//    this->setIndexWidget(model->index(0,0),lab_resulotion);
//    QString info("<a href=\"about://add\">Add</a>&nbsp;<a href=\"about://remove?%1\">Remove</a>&nbsp;<a href=\"about://alert?%1\">Alert</a>");
}

void MyTableView::setData(QVector<QStringList> *data,QString arg1,QString arg2)
{
    if(data->isEmpty()){
        cout << "data is empty!";
        return;
    }
    model->clear();
    model->setRowCount(arg1.toInt());
    model->setColumnCount(3);
    cout << "xxxxxxxxxxxx0";
    QString Test = arg1;
    QString Fail = QString::number(arg1.toInt() - arg2.toInt());
    QStringList hHead;
    hHead << QString("Test(%1)").arg(Test)
          << QString("Result(Pass:%1/Fail:%2)").arg(arg2).arg(Fail)
          << QString("Resulotion");
    model->setHorizontalHeaderLabels(hHead);
    cout << "xxxxxxxxxxxx1";
//    QString link = "<a href='https://www.baidu.com'>baidu;<a href='https://online.mediatek.com'>MTK";
    for(int row=0;row<data->size();row++){
        cout << data->at(row);
        lab_resulotion->setText(data->at(row).at(2));
        model->setItem(row,0,new QStandardItem(data->at(row).at(0)));
        model->setItem(row,1,new QStandardItem(data->at(row).at(1)));
        this->setIndexWidget(model->index(row,2),lab_resulotion);
    }
    cout << "xxxxxxxxxxxx2";
    this->resizeColumnsToContents();
}


