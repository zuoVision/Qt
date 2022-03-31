#include "mytableview.h"

#include <QDebug>
#include <QLabel>
#include <QTextEdit>

#define MY_TAG "MyTableView"
#define cout    qDebug() << MY_TAG <<"[" << __FUNCTION__ <<"]"

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
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setAutoScroll(true);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    html_format="<a href=\"%1\">%2";
    lab_resulotion = new QLabel();
    lab_resulotion->setOpenExternalLinks(true);
    model->setColumnCount(3);
    model->setRowCount(5);
    model->setHorizontalHeaderLabels(QStringList()<<"Test"<<"Result"<<"Resulotion");
    lab_resulotion->setText("<a href=\'https://www.baidu.com\'>baidu");
    this->setIndexWidget(model->index(0,0),lab_resulotion);
//    QString info("<a href=\"about://add\">Add</a>&nbsp;<a href=\"about://remove?%1\">Remove</a>&nbsp;<a href=\"about://alert?%1\">Alert</a>");
}

void MyTableView::setData(QVector<QStringList> *data)
{
    if(data->isEmpty()){
        cout << "data is empty!";
        return;
    }
    QString link = "<a href=\'https://www.baidu.com\'>baidu";
    for(int row=0;row<data->size();row++){
//        cout << data->at(row);
//        link = html_format.arg(data->at(row).at(3)).arg(data->at(row).at(2));
//        link = data->at(row).at(2);
//        cout << link;
        lab_resulotion->setText(link);
        model->setItem(row,0,new QStandardItem(data->at(row).at(0)));
        model->setItem(row,1,new QStandardItem(data->at(row).at(1)));
        this->setIndexWidget(model->index(row,2),lab_resulotion);
    }
    this->resizeColumnsToContents();
}


