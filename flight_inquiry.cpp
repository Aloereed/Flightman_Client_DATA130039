#include "flight_inquiry.h"
#include "ui_flight_inquiry.h"
#include "flight_inquiry_citys_and_date.h"
#include "mainclientwindow.h"
#include "flight_inquiry_flightid.h"
#include <QDebug>

flight_inquiry::flight_inquiry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::flight_inquiry)
{
    ui->setupUi(this);
    ui->groupBox->setTitle(tr("Query Method:"));
    ui->hint_label->setText(tr("Welcome to query."));
    ui->QuerypushButton->setText(tr("Query"));
    ui->BackpushButton->setText(tr("Back"));
    ui->CancelpushButton->setText(tr("Cancel"));
    ui->CitysDate_radioButton->setText(tr("Citys and Date"));
    ui->CitysradioButton->setText(tr("Citys"));
    ui->FlightIDradioButton->setText(tr("Flight ID"));

    ui->CitysDate_radioButton->click();
}

flight_inquiry::~flight_inquiry()
{
    delete ui;
}

void flight_inquiry::on_CancelpushButton_clicked()
{
    qDebug()<<"You decide to quit."<<endl;
    QApplication::quit();
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
        flight_inquiry_citys_and_date * flt_cd = new flight_inquiry_citys_and_date(nullptr,"","","CD");
        flt_cd->show();
        this->close();
    }
    else if(ui->CitysradioButton->isChecked()){
        qDebug()<<"Enter the C mode."<<endl;
        flight_inquiry_citys_and_date * flt_c = new flight_inquiry_citys_and_date(nullptr,"","","C");
        flt_c->show();
        this->close();
    }
    else{
        flight_inquiry_flightID *flt_id = new flight_inquiry_flightID();
        flt_id->show();
        this->close();
    }
}
