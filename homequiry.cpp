#include "homequiry.h"
#include "ui_homequiry.h"
#include "flight_inquiry.h"
#include "flight_inquiry_citys_and_date.h"
#include "account_and_orders.h"
#include "chineseletterhelper.h"
extern account_and_orders* acct;
HomeQuiry::HomeQuiry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeQuiry) {
    ui->setupUi(this);
    ui->widget_2->setMaximumHeight(ui->dateEdit->height()*2);
    ui->tabWidget->setMaximumHeight(this->height()/2);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->label->setText(ui->dateEdit->date().toString("ddd"));
    ui->pushButton->setIconSize(ui->pushButton->rect().size());
    ui->dateEdit_2->setDate(QDate::currentDate());
    ui->dateEdit_3->setDate(QDate::currentDate().addDays(1));
    ui->label_3->setText(ui->dateEdit_2->date().toString("ddd"));
    ui->label_4->setText(ui->dateEdit_3->date().toString("ddd"));
    ui->pushButton_5->setIconSize(ui->pushButton->rect().size());

}

HomeQuiry::~HomeQuiry() {
    delete ui;
}

void HomeQuiry::on_pushButton_clicked() {
    QString temp=ui->lineEdit->text();
    ui->lineEdit->setText(ui->lineEdit_2->text());
    ui->lineEdit_2->setText(temp);
}

void HomeQuiry::on_pushButton_5_clicked() {
    QString temp=ui->lineEdit_5->text();
    ui->lineEdit_5->setText(ui->lineEdit_6->text());
    ui->lineEdit_6->setText(temp);
}

void HomeQuiry::on_dateEdit_dateChanged(const QDate &date) {
    ui->label->setText(ui->dateEdit->date().toString("ddd"));
}

void HomeQuiry::on_dateEdit_2_dateChanged(const QDate &date) {
    ui->label_3->setText(ui->dateEdit_2->date().toString("ddd"));
}

void HomeQuiry::on_dateEdit_3_dateChanged(const QDate &date) {
    ui->label_4->setText(ui->dateEdit_3->date().toString("ddd"));
}

void HomeQuiry::on_pushButton_6_clicked() {
    flight_inquiry * flt_in;
    if(acct) {
        acct->on_buyticket_pushButton_clicked();
    } else {
        flt_in= new flight_inquiry(nullptr,"","","");
        flt_in->show();
    }


}

void HomeQuiry::on_pushButton_2_clicked() {
    flight_inquiry_citys_and_date * flt_cd;
    if(acct) {
        flt_cd = new flight_inquiry_citys_and_date(nullptr,acct->UserID
                ,acct->Password,"CD",acct->Name);
    } else {
        flt_cd = new flight_inquiry_citys_and_date(nullptr,""
                ,"","CD","");
    }
    //flt_cd->ui->DepartureCity->setText(ui->lineEdit->text());
    flt_cd->setDepartureCity(ChineseLetterHelper::GetPinyins(ui->lineEdit->text())) ;
    flt_cd->setArrivalCity(ChineseLetterHelper::GetPinyins(ui->lineEdit_2->text())) ;
    flt_cd->setDate(ui->dateEdit->date()) ;
    flt_cd->on_pushButton_clicked();
    flt_cd->show();
}
void HomeQuiry::on_pushButton_3_clicked() {
    flight_inquiry_citys_and_date * flt_cd,*flt_cd2;
    if(acct) {
        flt_cd = new flight_inquiry_citys_and_date(nullptr,acct->UserID
                ,acct->Password,"CD",acct->Name);
        flt_cd2 = new flight_inquiry_citys_and_date(nullptr,acct->UserID
                ,acct->Password,"CD",acct->Name);
    } else {
        flt_cd = new flight_inquiry_citys_and_date(nullptr,""
                ,"","CD","");
        flt_cd2 = new flight_inquiry_citys_and_date(nullptr,""
                ,"","CD","");
    }
    //flt_cd->ui->DepartureCity->setText(ui->lineEdit->text());
    flt_cd->setDepartureCity(ChineseLetterHelper::GetPinyins(ui->lineEdit_5->text())) ;
    flt_cd->setArrivalCity(ChineseLetterHelper::GetPinyins(ui->lineEdit_6->text())) ;
    flt_cd->setDate(ui->dateEdit_2->date()) ;
    flt_cd->on_pushButton_clicked();
    flt_cd->show();
    flt_cd2->setDepartureCity(ChineseLetterHelper::GetPinyins(ui->lineEdit_6->text())) ;
    flt_cd2->setArrivalCity(ChineseLetterHelper::GetPinyins(ui->lineEdit_5->text())) ;
    flt_cd2->setDate(ui->dateEdit_3->date()) ;
    flt_cd2->on_pushButton_clicked();
    flt_cd2->show();
}
