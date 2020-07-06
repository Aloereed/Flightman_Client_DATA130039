#include "ticket_refund_confirm.h"
#include "ui_ticket_refund_confirm.h"
#include "account_and_orders.h"
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>

extern account_and_orders * acct;

ticket_refund_confirm::ticket_refund_confirm(QWidget *parent,QString UserID,float newBalance,
                                             QString ticketID,float refundMoney,
                                             QString flightID,QString dep_datetime,
                                             QString order_start,QString order_end,QString classType):
    QWidget(parent),
    ui(new Ui::ticket_refund_confirm)
{
    ui->setupUi(this);

    ui->groupBox->setTitle(tr("Authentication:"));
    ui->label_UserID->setText(tr("User ID :"));
    ui->lineEdit_UserID->setPlaceholderText(tr("Please Enter Your ID."));
    ui->lineEdit_UserID->setEchoMode(QLineEdit::Normal);

    ui->label_Pwd->setText(tr("Password :"));
    ui->lineEdit_Pwd->setPlaceholderText(tr("Please Enter Your Password."));
    ui->lineEdit_Pwd->setEchoMode(QLineEdit::Password);

    ui->pushButton_confirm->setText(tr("Confirm"));
    ui->pushButton_cancel->setText(tr("Cancel"));

    this->UserID = UserID;
    this->flightID = flightID;
    this->ticketID = ticketID;
    this->classType = classType;
    this->order_end = order_end;
    this->order_start = order_start;
    this->newBalance = newBalance;
    this->refundMoney = refundMoney;
    this->dep_datetime = dep_datetime;
}

ticket_refund_confirm::~ticket_refund_confirm()
{
    delete ui;
}

void ticket_refund_confirm::on_pushButton_cancel_clicked()
{
    this->close();
}

void ticket_refund_confirm::on_pushButton_confirm_clicked()
{
    QString UserID = ui->lineEdit_UserID->text();
    QString Pwd = ui->lineEdit_Pwd->text();
    if(UserID == "" || Pwd == ""){
        QMessageBox::critical(this,tr("WRONG"),tr("Your input is not complete."));
        return;
    }
    QByteArray bytePwd = Pwd.toLatin1(); //trasnform Password for safety.
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    Pwd = bytePwdMd5.toHex();

    QString sql = QString("SELECT * FROM `user` WHERE ID='%1' AND `password`='%2'").arg(UserID).arg(Pwd);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){//账户存在，执行购买操作
        sql = QString("BEGIN; "
//                      "CALL balanceRefresh('%1',%2); "
                      "CALL TicketsRefundInsertion('%3',%4); "
                      "CALL TicketsRefundLeftNumRefresh('%5','%6',%7,%8,%9); "
                      "COMMIT; ").arg(this->ticketID)  //.arg(this->UserID).arg(this->newBalance).
                .arg(this->refundMoney).arg(this->flightID).arg(this->dep_datetime)
                .arg(this->order_start).arg(this->order_end).arg(this->classType);
        query->clear();
        qDebug()<<sql<<endl;
        if(query->exec(sql)){ //上述退票过程执行成功
            QMessageBox::information(this,tr("Hint:"),tr("Refund successfully"));
            this->close();
        }
        else{//上述退票过程出现问题，更新撤回
            QMessageBox::information(this,tr("Hint:"),tr("Something is wrong. Please try again..."));
            acct->setMoney(acct->getMoney()-refundMoney);
        }
    }
}
