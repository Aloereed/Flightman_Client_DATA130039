#ifndef TICKET_PURCHASE_H
#define TICKET_PURCHASE_H

#include <QWidget>

namespace Ui {
class Ticket_Purchase;
}

class Ticket_Purchase : public QWidget
{
    Q_OBJECT

public:
    explicit Ticket_Purchase(QWidget *parent = nullptr,QString dep_date="",QString flightID="",QString schedule=""
            ,QString dep_airportName="",QString dep_city="",QString dep_time="",QString arv_airportName=""
            ,QString arv_city="",QString arv_time="",QString orderstart="",QString orderend="");
    ~Ticket_Purchase();

    QString PrimalPriceQuery(QString flightID,QString classType, QString order_start,QString order_end);
    // QString FinalPriceQuery()
    QString DiscountQuery(QString flightID,QString depDate,QString classType);
    QString CompanyQuery(QString flightID);
    QString TicketsLeftQuery(QString flightID,QString depDate,QString order_start,QString order_end,QString classType);
    void TicketsLeftRefresh();
    void BalanceRefresh();


private slots:
    void on_pushButton_clicked();

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
};

#endif // TICKET_PURCHASE_H
