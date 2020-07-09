#ifndef ACCOUNT_AND_ORDERS_H
#define ACCOUNT_AND_ORDERS_H

#include <QWidget>
#include <QString>
#include <QSqlQueryModel>
#include <QModelIndex>
#include <QVariant>
#include <QHash>
#include <QTimer>
#include "homequiry.h"



namespace Ui {
class account_and_orders;
}

class account_and_orders : public QWidget {
    Q_OBJECT
    friend class HomeQuiry;
  public:
    explicit account_and_orders(QWidget *parent = nullptr, QString ID = "", QString Pwd = "");
    ~account_and_orders();

    void setUserID(QString ID);
    void setPassword(QString Pwd);
    void setMoney(float Money);
    void setStatus(int status);
    void setChangeFlightDate(int ChangeFlightDate);
    void setRebooking_newBalance(float balance){this->Rebooking_newBalance=balance;}
    void setRebooking_ticketID(QString ticketID){ this->Rebooking_ticketID=ticketID;}
    void setRebooking_refundMoney(float refundMoney){this->Rebooking_refundMoney = refundMoney;}
    void setRebooking_flightID(QString flightID){this->Rebooking_flightID=flightID;}
    void setRebooking_dep_datetime(QString dep_datetime){this->Rebooking_dep_datetime=dep_datetime;}
    void setRebooking_order_start(QString orderS){this->Rebooking_order_start=orderS;}
    void setRebooking_order_end(QString orderE){this->Rebooking_order_end=orderE;}
    void setRebooking_classType(QString classType){this->Rebooking_classType=classType;}

    float getMoney();
    float ticketActualRefundQuery(float actualPay,QString dep_datetime);
    QString getUserID();
    QString seatIDQuery(QString flightID,QString dep_date,QString passengerID);
    QString ticketOrderStartQuery(QString ticketID);
    QString ticketOrderEndQuery(QString ticketID);
    QString ticketActualPayQuery(QString ticketID);
    QString airportCityQuery(QString airportID);
    QHash<QString,QString> HashSeatsOfUser(QString passengerID);
    QHash<QString,QString> getHashSeatsOfUser();
    QTimer timer;
    bool IsCheckedIn(QString flightID, QString dep_date,QString passengerID);

    void BalanceRefresh();

    int getMembership();
    int getStatus();
    QString getPassword();
    float getRebooking_newBalance(){return this->Rebooking_newBalance;}
    QString getRebooking_ticketID(){return this->Rebooking_ticketID;}
    float getRebooking_refundMoney(){return this->Rebooking_refundMoney;}
    QString getRebooking_flightID(){return this->Rebooking_flightID;}
    QString getRebooking_dep_datetime(){return this->Rebooking_dep_datetime;}
    QString getRebooking_order_start(){return this->Rebooking_order_start;}
    QString getRebooking_order_end(){return this->Rebooking_order_end;}
    QString getRebooking_classType(){return this->Rebooking_classType;}

  public slots:
    void on_buyticket_pushButton_clicked();
    void checkNewMsg();

  private slots:
    void on_Buymem_pushButton_clicked();

    void on_Recharge_pushButton_clicked();

    void on_Refresh_pushButton_clicked();

    void on_coming_tableView_clicked(const QModelIndex &index);

    // void on_buyticket_pushButton_clicked();


    void on_logoutButton_clicked();

    void on_MessageBox_pushButton_clicked();

private:
    Ui::account_and_orders *ui;

    QString UserID;
    QString Password;
    float Money;
    int Membership;
    QString Name;
    QString seatID;
    int status;
    int ChangeFlightDate;
    QHash<QString,QString> hash_seatsInfoOfUser;

    float Rebooking_newBalance;
    QString Rebooking_ticketID;
    float Rebooking_refundMoney;
    QString Rebooking_flightID;
    QString Rebooking_dep_datetime;
    QString Rebooking_order_start;
    QString Rebooking_order_end;
    QString Rebooking_classType;

//    ticket_refund_confirm *refund_interface = new ticket_refund_confirm(nullptr,this->UserID,newBalance,ticketID
//            ,refundMoney,flightID,dep_datetime
//            ,order_start,order_end,classType,"1");
};

class FinishedOrderModel:public QSqlQueryModel {
    Q_OBJECT
  public:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class ComingOrderModel:public QSqlQueryModel {
    Q_OBJECT
  public:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class CancelledOrderModel:public QSqlQueryModel {
    Q_OBJECT
  public:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};



#endif // ACCOUNT_AND_ORDERS_H
