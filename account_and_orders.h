#ifndef ACCOUNT_AND_ORDERS_H
#define ACCOUNT_AND_ORDERS_H

#include <QWidget>
#include <QString>
#include <QSqlQueryModel>
#include <QModelIndex>
#include <QVariant>



namespace Ui {
class account_and_orders;
}

class account_and_orders : public QWidget
{
    Q_OBJECT

public:
    explicit account_and_orders(QWidget *parent = nullptr, QString ID = "", QString Pwd = "");
    ~account_and_orders();

    void setUserID(QString ID);
    void setPassword(QString Pwd);
    void setMoney(float Money);

    float getMoney();
    QString getUserID();

    void BalanceRefresh();

    int getMembership();

private slots:
    void on_Buymem_pushButton_clicked();

    void on_Recharge_pushButton_clicked();

    void on_Refresh_pushButton_clicked();

    void on_coming_tableView_clicked(const QModelIndex &index);

    void on_buyticket_pushButton_clicked();

private:
    Ui::account_and_orders *ui;

    QString UserID;
    QString Password;
    float Money;
    int Membership;
    QString Name;
};

class FinishedOrderModel:public QSqlQueryModel{
public:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class ComingOrderModel:public QSqlQueryModel{
public:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class CancelledOrderModel:public QSqlQueryModel{
public:
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};



#endif // ACCOUNT_AND_ORDERS_H
