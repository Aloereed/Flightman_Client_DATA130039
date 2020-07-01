#include "account_and_orders.h"
#include "ui_account_and_orders.h"
#include "recharge.h"
#include <QDebug>
#include <QWidget>
#include <QSqlError>
#include <QMessageBox>
#include <QWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSqlQuery>
#include <QCryptographicHash>

//构造对象时，必须要给参数：ID 和 Pwd
//此处传入的Pwd是经过MD5转换后的密码，与数据库中所存储的密码对应
extern account_and_orders * acct;

account_and_orders::account_and_orders(QWidget *parent,QString ID,QString Pwd) : QWidget(parent), ui(new Ui::account_and_orders)
{
    ui->setupUi(this);

    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    //根据登录界面给定的有效用户信息初始化账户界面

    this->UserID=ID;
    this->Password=Pwd;

    QString sql_user = QString("SELECT * FROM user WHERE ID = '%1' AND password = '%2'").arg(ID).arg(Pwd);
    QSqlQuery * query_user = new QSqlQuery();
    query_user->exec(sql_user);
    QString money_string = QString("");
    int money = 0;
    int membership = 0;
    QString name = QString("");
    if(query_user->next()){
        money_string = query_user->value(3).toString();
        money = query_user->value(3).toInt();
        membership = query_user->value(2).toInt();
        name = query_user->value(1).toString();
    }

    this->Name = name;
    this->Money = money;
    this->Membership = membership;

    QString greeting = QString("Hello, %1").arg(this->Name);
    ui->User_label->setText(greeting);
    ui->account_label->setText(tr("Account Balance: ") + money_string + tr("￥"));

    if(membership == 1){
        ui->memInfo_label->setText(tr("Membership: √"));
        ui->Buymem_pushButton->setDisabled(true);
        //成为会员之后不需要再购买会员
    }
    else{
        ui->memInfo_label->setText(tr("Membership: ×"));
    }

    ui->Buymem_pushButton->setText(tr("Pay for Membership (100￥)"));
    ui->Recharge_pushButton->setText(tr("Recharge"));
    ui->Refresh_pushButton->setText(tr("Refresh"));
    ui->buyticket_pushButton->setText("Pay for Flights");

    ui->orders->setTabText(0,tr("Finished Orders"));
    ui->orders->setTabText(1,tr("Coming Orders"));
    ui->orders->setTabText(2,tr("Cancelled Orders"));

}

account_and_orders::~account_and_orders()
{
    delete ui;
}

void account_and_orders::setMoney(int Money)
{
    this->Money = Money;
}

int account_and_orders::getMoney()
{
    return this->Money;
}

QString account_and_orders::getUserID()
{
    return this->UserID;
}

void account_and_orders::BalanceRefresh()
{
    QString money_string = QString("%1").arg(this->Money);
    ui->account_label->setText(tr("Account Balance: ") + money_string + tr("￥"));
}

void account_and_orders::on_Buymem_pushButton_clicked()
{
    if (this->Money < 100){
        QMessageBox::information(this,tr("Hint"),tr("Sorry, your balance is not enough."));
        return;
    }

    int money = this->Money - 100;

    //更新数据库中的信息
    QString sql_user = QString("UPDATE user "
                               "SET "
                               "membership = 1, "
                               "account = %1 "
                               "WHERE ID = '%2' ").arg(money).arg(this->UserID);
    QSqlQuery * query_user = new QSqlQuery();

    qDebug()<<sql_user <<endl;
    //更新界面
    bool ok = query_user->exec(sql_user);
    if(ok){
        this->Money -= 100;  //扣除会员费
        this->BalanceRefresh();
        ui->memInfo_label->setText(tr("Membership: √"));
        ui->Buymem_pushButton->setDisabled(true);

        QMessageBox::information(this,tr("Hint:"),tr("Your payment has been done."));
    }
    else{
        QMessageBox::information(this,tr("Hint:"),tr("Something wrong with your payment."));
    }


}

void account_and_orders::on_Recharge_pushButton_clicked()
{
    recharge * rec = new recharge();
    rec->show();
}
