#ifndef RECHARGE_H
#define RECHARGE_H

#include <QWidget>
#include <QString>
#include "account_and_orders.h"

namespace Ui {
class recharge;
}

class recharge : public QWidget
{
    Q_OBJECT

public:
    explicit recharge(QWidget *parent = nullptr);
    ~recharge();

private slots:
    void on_Back_pushButton_clicked();

    void on_Ok_pushButton_clicked();

private:
    Ui::recharge *ui;
};

#endif // RECHARGE_H
