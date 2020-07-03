#include "flight_inquiry_flightid.h"
#include "ui_flight_inquiry_flightid.h"
#include "flight_inquiry.h"
#include <QDebug>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>

flight_inquiry_flightID::flight_inquiry_flightID(QWidget *parent,QString UserID,QString Password,QString Name) :
    QWidget(parent),
    ui(new Ui::flight_inquiry_flightID)
{
    ui->setupUi(this);
    QFile styleFile(":/style.qss");//使用qss文件
    styleFile.open(QIODevice::ReadOnly);//只读
    QString setStyleSheet(styleFile.readAll());//读取所有的
    ui->tableView->setStyleSheet(setStyleSheet);//设置样式
    this->UserID = UserID;
    this->Password = Password;
    this->Name = Name;

    ui->hint_label->setText(tr("Flight ID"));
    ui->flighID_lineEdit->setPlaceholderText(tr("Please enter the flight ID you want to query."));
    ui->flighID_lineEdit->setEchoMode(QLineEdit::Normal);
    ui->Back_pushButton->setText(tr("Back"));
    ui->flightInfo_label->setText(tr("Flight Information:"));
    ui->Confirm_pushButton->setText(tr("Confirm"));
    ui->Cancel_pushButton->setText(tr("Cancel"));

    if (UserID==""){
        ui->UserStatus->setText(tr("User Information: Not logged in"));
    }
    else{
        ui->UserStatus->setText("User Information: "+Name);
    }
}

flight_inquiry_flightID::~flight_inquiry_flightID()
{
    delete ui;
}

void flight_inquiry_flightID::on_Cancel_pushButton_clicked()
{
    qDebug()<<"You decied to quit."<<endl;
    this->close();
}

void flight_inquiry_flightID::on_Back_pushButton_clicked()
{
    flight_inquiry * flt_inq = new flight_inquiry(nullptr,this->UserID,this->Password,this->Name);
    flt_inq->show();
    this->close();
}



void flight_inquiry_flightID::on_Confirm_pushButton_clicked()
{
    QString flightID = ui->flighID_lineEdit->text();
    if (flightID == ""){
        QMessageBox::critical(this,tr("Critical"),tr("Incomplete input."));
        return;
    }
    QString sql = QString("CALL flight_query_ID('%1')").arg(flightID);

    qDebug()<<sql<<endl;

    MyQueryMode_fid *model = new MyQueryMode_fid();
    model->setQuery(sql);
    model->setHeaderData(0,Qt::Horizontal,QString::fromUtf8(tr("Departure Date").toUtf8()));
    model->setHeaderData(1,Qt::Horizontal,QString::fromUtf8(tr("Flight ID").toUtf8()));
    model->setHeaderData(2,Qt::Horizontal,QString::fromUtf8(tr("Schedule").toUtf8()));
    model->setHeaderData(3,Qt::Horizontal,QString::fromUtf8(tr("Departure Airport ID").toUtf8()));
    model->setHeaderData(4,Qt::Horizontal,QString::fromUtf8(tr("Departure Airport").toUtf8()));
    model->setHeaderData(5,Qt::Horizontal,QString::fromUtf8(tr("Departure City").toUtf8()));
    model->setHeaderData(6,Qt::Horizontal,QString::fromUtf8(tr("Departure Time").toUtf8()));
    model->setHeaderData(7,Qt::Horizontal,QString::fromUtf8(tr("Departure Positon").toUtf8()));
    model->setHeaderData(8,Qt::Horizontal,QString::fromUtf8(tr("Arrival Airport ID").toUtf8()));
    model->setHeaderData(9,Qt::Horizontal,QString::fromUtf8(tr("Arrival Airport").toUtf8()));
    model->setHeaderData(10,Qt::Horizontal,QString::fromUtf8(tr("Arrival City").toUtf8()));
    model->setHeaderData(11,Qt::Horizontal,QString::fromUtf8(tr("Arrival Time").toUtf8()));
    model->setHeaderData(12,Qt::Horizontal,QString::fromUtf8(tr("Arrival Positon").toUtf8()));
    model->insertColumn(13); // 插入购买列
    model->setHeaderData(13,Qt::Horizontal,QString::fromUtf8(tr("Booking").toUtf8()));
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
}


QVariant MyQueryMode_fid::data(const QModelIndex &item, int role) const{
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::BackgroundColorRole){
        if(item.column()==13)
            return QVariant::fromValue(QColor(225,225,225));
    }
    if (role == Qt::DisplayRole){
        if(item.column()==7 || item.column()==12){
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("始发站")));
            else if(QSqlQueryModel::data(item).toInt()==-1)
                return QVariant::fromValue(QString(tr("终点站")));
            else{
                QString order = QSqlQueryModel::data(item).toString();
                QString info = "第" + order + "中转站";
                return QVariant::fromValue(info);
            }
        }
        if(item.column()==13)
            return QVariant::fromValue(tr("Booking"));
        }
        return value;
}




void flight_inquiry_flightID::on_tableView_clicked(const QModelIndex &index)
{
    if (index.isValid()&&index.column()==13){ //点击订票键，打开相关的UI界面，显示价格并进行购票操作
        //如果没有登陆，则提示登陆账户后再买票，并结束该界面，进入到登陆界面
        //如果登陆了，则打开购票信息框
        qDebug()<<"你刚刚点击了订票按钮"<<endl;
        qDebug()<<"你可以试试先登陆账户！"<<endl;
    }
}
