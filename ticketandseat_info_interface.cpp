#include "ticketandseat_info_interface.h"
#include "ui_ticketandseat_info_interface.h"

ticketANDseat_info_interface::ticketANDseat_info_interface(QWidget *parent,QString ticketID,QString price,QString flightID,
                                                           QString dep_airport,QString arv_airport,QString dep_datetime,
                                                           QString classType,QString seatID) :
    QWidget(parent),
    ui(new Ui::ticketANDseat_info_interface)
{
    ui->setupUi(this);
    ui->groupBox->setTitle(tr("Ticket Information:"));
    ui->label_ticketID->setText(tr("Ticket ID:")+ticketID);
    ui->label_actualPay->setText(tr("Price:")+price);
    ui->label_flightID->setText(flightID);
    ui->label_depAirport->setText(dep_airport);
    ui->label_arvAirport->setText(arv_airport);
    ui->label_depDateTime->setText(tr("Departure Time:")+dep_datetime);
    ui->label_class->setText(tr("Class Type:")+classType);
    ui->label_seat->setText(tr("Seat ID:")+seatID);
    ui->pushButton_cancel->setText(tr("Cancel"));
}

ticketANDseat_info_interface::~ticketANDseat_info_interface()
{
    delete ui;
}

void ticketANDseat_info_interface::on_pushButton_cancel_clicked()
{
    this->close();
}
