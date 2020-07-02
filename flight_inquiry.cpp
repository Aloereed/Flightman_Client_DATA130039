#include "flight_inquiry.h"
#include "ui_flight_inquiry.h"
#include "flight_inquiry_citys_and_date.h"
#include "mainclientwindow.h"
#include "flight_inquiry_flightid.h"
#include <QDebug>
#include <QSqlQuery>

flight_inquiry::flight_inquiry(QWidget *parent,QString UserID,QString Pwd,QString Name) :
    QWidget(parent),
    ui(new Ui::flight_inquiry)
{
    ui->setupUi(this);
    ui->groupBox->setTitle(tr("Query Mode:"));
    ui->hint_label->setText(tr("Welcome to query."));
    ui->QuerypushButton->setText(tr("Query"));
    ui->BackpushButton->setText(tr("Back"));
    ui->CancelpushButton->setText(tr("Cancel"));
    ui->CitysDate_radioButton->setText(tr("Citys and Date"));
    ui->CitysradioButton->setText(tr("Citys"));
    ui->FlightIDradioButton->setText(tr("Flight ID"));
    ui->label->setText("");

    ui->CitysDate_radioButton->click();

    this->UserID = UserID;
    this->Pwd = Pwd;
    this->Name = Name;
    this->greeting(UserID,Pwd);

    if(this->UserID != ""){
        ui->BackpushButton->hide();
    }
}

flight_inquiry::~flight_inquiry()
{
    delete ui;
}

void flight_inquiry::greeting(QString ID, QString Password)
{
        if(ID != "" && Password != ""){
            QString sql = QString("SELECT 1 FROM user WHERE ID='%1' AND password='%2'").arg(ID).arg(Password);
            QSqlQuery *query = new QSqlQuery();
            query->exec(sql);
            if(query->next()){
                //若查询成功，该账户和密码有效，提供欢迎语
                QString gretting_words = "Hello "+this->Name+" !";
                this->ui->label->setText(gretting_words);
            }
            else{
                //查询失败，则认为是游客登陆
                this->UserID = "";
                this->Pwd = "";
                this->Name = "";
            }
        }else{//输入账号和密码有误，清空账户信息，认为是游客登陆
            this->UserID = "";
            this->Pwd = "";
            this->Name = "";
        }
}

void flight_inquiry::on_CancelpushButton_clicked()
{
    qDebug()<<"You decide to quit."<<endl;
    this->close();
}


void flight_inquiry::on_BackpushButton_clicked()
{
    MainClientWindow * mainclientwindow = new MainClientWindow();
    QApplication::processEvents();
    mainclientwindow->show();
    QApplication::processEvents();
    this->close();
}

void flight_inquiry::on_QuerypushButton_clicked()
{
    if(ui->CitysDate_radioButton->isChecked()){
        qDebug()<<"Enter the CD mode."<<endl;
        flight_inquiry_citys_and_date * flt_cd = new flight_inquiry_citys_and_date(nullptr,this->UserID
                                                                                   ,this->Pwd,"CD",this->Name);
        flt_cd->show();
        this->close();
    }
    else if(ui->CitysradioButton->isChecked()){
        qDebug()<<"Enter the C mode."<<endl;
        flight_inquiry_citys_and_date * flt_c = new flight_inquiry_citys_and_date(nullptr,this->UserID
                                                                                  ,this->Pwd,"C",this->Name);
        flt_c->show();
        this->close();
    }
    else{
        flight_inquiry_flightID *flt_id = new flight_inquiry_flightID(nullptr,this->UserID,this->Pwd,this->Name);
        flt_id->show();
        this->close();
    }
}
