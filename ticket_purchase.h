#ifndef TICKET_PURCHASE_H
#define TICKET_PURCHASE_H

#include "flight_inquiry_citys_and_date.h"
#include "flight_inquiry_flightid.h"
#include <QWidget>

namespace Ui {
class Ticket_Purchase;
}

class Ticket_Purchase : public QWidget
{
    Q_OBJECT

public:
    explicit Ticket_Purchase(QWidget *parent = nullptr
            ,flight_inquiry_citys_and_date *parent1=nullptr,flight_inquiry_flightID *parent2=nullptr
            ,QString dep_date="",QString flightID="",QString schedule=""
            ,QString dep_airportName="",QString dep_city="",QString dep_time="",QString arv_airportName=""
            ,QString arv_city="",QString arv_time="",QString orderstart="",QString orderend="");
    ~Ticket_Purchase();

    QString PrimalPriceQuery(QString flightID,QString classType, QString order_start,QString order_end);
    // QString FinalPriceQuery()
    QString DiscountQuery(QString flightID,QString depDate,QString classType);
    QString CompanyQuery(QString flightID);
    QString TicketsLeftQuery(QString flightID,QString depDate,QString order_start,QString order_end,QString classType);
    QString getRandomString(int length);

    void TicketsLeftRefresh();
    void BalanceRefresh();

    // 这四个函数用于更新数据库中与购票相关连的表信息
    void Payment(flight_inquiry_citys_and_date *parent1,flight_inquiry_flightID *parent2,
                 QString UserID,QString balance,QString price,
                 QString flightID,QString dep_date,QString dep_time,QString order_start,QString order_end,
                 QString classType,QString companyID); //支付机票价格，余额更新
//    void TicketsRecordInsertion();
//    void TicketsPurchaseRecordInsertion();
//    void TicketsLeftNumRefresh();


private slots:
    void on_pushButton_clicked();

    void on_pushButton_Refresh_clicked();

private:
    Ui::Ticket_Purchase *ui;
    QString depature_date;
    QString flightID;
    QString schedule;
    QString depature_airportName;
    QString departure_city;
    QString departure_time;
    QString arrival_airportName;
    QString arrival_city;
    QString arrival_time;
    QString orderstart;
    QString orderend;
    flight_inquiry_citys_and_date *ptr_CD;
    flight_inquiry_flightID *ptr_flightID;
};

#endif // TICKET_PURCHASE_H
