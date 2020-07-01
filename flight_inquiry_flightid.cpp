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
#include <QSqlQueryModel>

flight_inquiry_flightID::flight_inquiry_flightID(QWidget *parent,QString UserID,QString Password) :
    QWidget(parent),
    ui(new Ui::flight_inquiry_flightID)
{
    ui->setupUi(this);
    this->UserID = UserID;
    this->Password = Password;

    ui->hint_label->setText(tr("Flight ID"));
    ui->flighID_lineEdit->setPlaceholderText(tr("Please enter the flight ID you want to query."));
    ui->flighID_lineEdit->setEchoMode(QLineEdit::Normal);
    ui->Back_pushButton->setText(tr("Back"));
    ui->flightInfo_label->setText(tr("Flight Information:"));
    ui->Confirm_pushButton->setText(tr("Confirm"));
    ui->Cancel_pushButton->setText(tr("Cancel"));

    if (UserID==""){
        ui->UserStatus->setText("User Information: Not logged in");
    }
    else{
        QString sql = QString("select name from [user] where ID = '%1'").arg(UserID);
        QSqlQuery * query = new QSqlQuery();
        query->exec(sql);
        if (query->next()){
            QString UserName = query->value(0).toString();
            ui->UserStatus->setText("User Information: "+UserName);
        }
        else{//即便有用户ID输入，但是数据库内无相关用户ID的信息，所以视为用户登录失败，显示未登录；
            ui->UserStatus->setText("User Information: Not logged in");
        }
    }
}

flight_inquiry_flightID::~flight_inquiry_flightID()
{
    delete ui;
}

void flight_inquiry_flightID::on_Cancel_pushButton_clicked()
{
    qDebug()<<"You decied to quit."<<endl;
    QApplication::quit();
}

void flight_inquiry_flightID::on_Back_pushButton_clicked()
{
    flight_inquiry * flt_inq = new flight_inquiry();
    flt_inq->show();
    this->close();
}
