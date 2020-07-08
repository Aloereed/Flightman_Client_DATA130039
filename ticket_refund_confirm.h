#ifndef TICKET_REFUND_CONFIRM_H
#define TICKET_REFUND_CONFIRM_H

#include <QWidget>

namespace Ui {
class ticket_refund_confirm;
}

class ticket_refund_confirm : public QWidget
{
    Q_OBJECT

public:
    explicit ticket_refund_confirm(QWidget *parent = nullptr,QString UserID="",float newBalance=0
            ,QString ticketID="",float refundMoney=0,QString flightID="",QString dep_datetime=""
            ,QString order_start="",QString order_end="",QString classType="",QString FromOrder="0");
    ~ticket_refund_confirm();

    void AutoConfirm();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_confirm_clicked();

private:
    Ui::ticket_refund_confirm *ui;

    QString UserID;
    float newBalance;
    QString ticketID;
    float refundMoney;
    QString flightID;
    QString dep_datetime;
    QString order_start;
    QString order_end;
    QString classType;
    QString FromOrder;
};

#endif // TICKET_REFUND_CONFIRM_H
