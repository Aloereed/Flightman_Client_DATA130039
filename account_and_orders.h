#ifndef ACCOUNT_AND_ORDERS_H
#define ACCOUNT_AND_ORDERS_H

#include <QWidget>
#include <QString>



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
    void setMoney(int Money);

    int getMoney();
    QString getUserID();

    void BalanceRefresh();

private slots:
    void on_Buymem_pushButton_clicked();

    void on_Recharge_pushButton_clicked();

private:
    Ui::account_and_orders *ui;

    QString UserID;
    QString Password;
    int Money;
    int Membership;
    QString Name;

};

#endif // ACCOUNT_AND_ORDERS_H
