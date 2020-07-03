#include "flight_inquiry_citys_and_date.h"
#include "ui_flight_inquiry_citys_and_date.h"
#include "flight_inquiry.h"
#include "mainclientwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>

flight_inquiry_citys_and_date::flight_inquiry_citys_and_date(
        QWidget *parent,QString UserID,QString Password, QString Mode,QString Name)
    : QWidget(parent), ui(new Ui::flight_inquiry_citys_and_date)
{
    ui->setupUi(this);
    QFile styleFile(":/style.qss");//使用qss文件
    styleFile.open(QIODevice::ReadOnly);//只读
    QString setStyleSheet(styleFile.readAll());//读取所有的
    ui->Flights->setStyleSheet(setStyleSheet);//设置样式

    this->UserID = UserID;
    this->Password = Password;
    this->Mode = Mode;
    this->Name = Name;

    if (UserID == ""){ //在之前的flight_inquiry已经对账户信息进行了筛选，此处不再重复
        ui->UserStatus->setText(tr("User Information: Not logged in"));
    }
    else{
        ui->UserStatus->setText("User Information: "+this->Name);
    }

    //只查询起降城市，不需要起飞日期
    if(Mode == 'C'){
        ui->label_3->hide();
        ui->dateEdit->hide();
    }

    ui->label->setText(tr("Departure City"));
    ui->DepatureCity->setEchoMode(QLineEdit::Normal);
    ui->DepatureCity->setPlaceholderText(tr("Please Enter Your Depature City."));

    ui->label_2->setText(tr("Arrival City"));
    ui->ArrivalCity->setEchoMode(QLineEdit::Normal);
    ui->ArrivalCity->setPlaceholderText(tr("Please Enter Your Arrival City."));

    ui->label_3->setText(tr("Departure Date"));
    ui->dateEdit->setDate(QDate::currentDate());

    ui->pushButton->setText(tr("Confirm"));
    ui->pushButton_2->setText(tr("Back"));
    ui->pushButton_3->setText(tr("Cancel"));
    ui->label_4->setText(tr("Flight Information:"));

    //layout()->setSizeConstraint(QLayout::SetFixedSize);

}

flight_inquiry_citys_and_date::~flight_inquiry_citys_and_date()
{
    delete ui;
}

//Btn: Back
void flight_inquiry_citys_and_date::on_pushButton_2_clicked()
{
    flight_inquiry * flt_inq = new flight_inquiry(nullptr,this->UserID,this->Password,this->Name);
    flt_inq->show();
    this->close();
}

//Btn:Cancel
void flight_inquiry_citys_and_date::on_pushButton_3_clicked()
{
    qDebug()<<"You decide to quit."<<endl;
    this->close();
}

//Btn:Confirm- for searching
void flight_inquiry_citys_and_date::on_pushButton_clicked()
{
    qDebug()<<Mode<<endl;
    QString dep_city = ui->DepatureCity->text();
    QString arv_city = ui->ArrivalCity->text();
    QString dep_date = ui->dateEdit->text();

    //允许只按照起降城市查询；
    if (dep_city == ""||arv_city == ""){
            QMessageBox::critical(this,tr("Critical"),tr("Incomplete input."));
            return;
    }

    if (this->Mode == "CD" && dep_date==""){
        QMessageBox::critical(this,tr("Critical"),tr("Incomplete input."));
        return;
    }

    //航班查询
    QString sql;
    if (this->Mode == "CD")
    {
        sql = QString("CALL flight_query_CD('%1','%2'"
                      ",'%3')").arg(dep_city).arg(arv_city).arg(dep_date);

    }
    else{
        sql = QString("CALL flight_query_C('%1','%2')").arg(dep_city).arg(arv_city);
    }
    // 查询结果的日期和时间是在NOW()的之后十分钟以及以后

    qDebug()<<sql<<endl;

    MyQueryMode *model = new MyQueryMode();
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
    ui->Flights->setModel(model);
    ui->Flights->resizeColumnsToContents();


    //还需要实现每一行的"购票"的按钮；

}

QVariant MyQueryMode::data(const QModelIndex &item, int role) const{
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


void flight_inquiry_citys_and_date::on_Flights_clicked(const QModelIndex &index)
{
    if (index.isValid()&&index.column()==13){ //点击订票键，打开相关的UI界面，显示价格并进行购票操作
        //如果没有登陆，则提示登陆账户后再买票，并结束该界面，进入到登陆界面
        //如果登陆了，则打开购票信息框
        qDebug()<<"你刚刚点击了订票按钮"<<endl;
    }
}
