#include "mytableview.h"

#include <QDebug>
#include <QLabel>
#include <QTextEdit>

#define MY_TAG  "MyTableView"
#define cout   qDebug() << MY_TAG <<"[" << __FUNCTION__ <<":" << __LINE__<<"]"

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

    model->setColumnCount(3);
    model->setRowCount(5);
    model->setHorizontalHeaderLabels(QStringList()<<"Test"<<"Result"<<"Resulotion");
}

void MyTableView::setData(QVector<QStringList> *data,QString arg1,QString arg2)
{
    if(data->isEmpty()){
        cout << "data is empty!";
        return;
    }
    model->setRowCount(arg1.toInt());
    model->setColumnCount(3);
    QString Test = arg1;
    QString Fail = QString::number(arg1.toInt() - arg2.toInt());
    QStringList hHead;
    hHead << QString("Test(%1)").arg(Test)
          << QString("Result(Pass:%1/Fail:%2)").arg(arg2).arg(Fail)
          << QString("Resulotion");
    model->setHorizontalHeaderLabels(hHead);
    for(int row=0;row<data->size();row++){
//        cout << data->at(row);
        QLabel *lab_resulotion = new QLabel();
        lab_resulotion->setOpenExternalLinks(true);
        lab_resulotion->setText(data->at(row).at(2));
        model->setItem(row,0,new QStandardItem(data->at(row).at(0)));
        model->setItem(row,1,new QStandardItem(data->at(row).at(1)));
        this->setIndexWidget(model->index(row,2),lab_resulotion);
    }
    this->resizeColumnsToContents();
}


