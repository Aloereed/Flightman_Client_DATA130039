#include "ticket_refund_confirm.h"
#include "ui_ticket_refund_confirm.h"
#include "account_and_orders.h"
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

extern account_and_orders * acct;
extern QSqlDatabase db;

ticket_refund_confirm::ticket_refund_confirm(QWidget *parent,QString UserID,float newBalance,
                                             QString ticketID,float refundMoney,
                                             QString flightID,QString dep_datetime,
                                             QString order_start,QString order_end,QString classType,
                                             QString FromOrder):
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
    this->FromOrder = FromOrder;

    if(this->FromOrder=="1"){ //不需要验证
        ui->lineEdit_UserID->setText(UserID);
        ui->lineEdit_Pwd->setText(acct->getPassword());
        this->AutoConfirm();
    }
}

ticket_refund_confirm::~ticket_refund_confirm()
{
    delete ui;
}

void ticket_refund_confirm::AutoConfirm()
{
    QString UserID = ui->lineEdit_UserID->text();
    QString Pwd = ui->lineEdit_Pwd->text();
    QByteArray bytePwd = Pwd.toLatin1(); //trasnform Password for safety.
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    Pwd = bytePwdMd5.toHex();
    QSqlQuery *query = new QSqlQuery();
    if(query->exec("BEGIN;")){
        bool sql_ok= true;
        //                          "CALL balanceRefresh('%1',%2); "
        //上悲观锁，避免两个事务同时修改时本应剩余票数加2，但实际上只是加1.（存储过程里）
        QString sql = QString(
                      "CALL TicketsRefundLeftNumRefresh('%1','%2',%3,%4,%5);"
                      "CALL TicketsRefundInsertion('%6',%7);").arg(this->flightID).arg(this->dep_datetime)
                .arg(this->order_start).arg(this->order_end).arg(this->classType)
                .arg(this->ticketID)  //.arg(this->UserID).arg(this->newBalance).
                .arg(this->refundMoney);
        QStringList sqlList = sql.split(";",QString::SkipEmptyParts);
        for (int i=0; i<sqlList.count() && sql_ok; i++)
        {
            qDebug()<<sqlList[i]<<endl;
            sql_ok &= query->exec(sqlList[i]);
        }
        if(sql_ok){
            query->exec("COMMIT;");
//            if(FromOrder=="0")
//                QMessageBox::information(this,tr("Hint:"),tr("Refund successfully"));
//            else
//                QMessageBox::information(this,tr("Hint:"),tr("You have finished your rebooking."));
//            this->close();
        }
        if(!sql_ok){//上述退票过程出现问题，更新撤回
            query->exec("ROLLBACK;");
            acct->setMoney(acct->getMoney()-refundMoney);
            QMessageBox::critical(0, "Error", query->lastError().text());
        }
    }
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
    if(UserID != acct->getUserID()){
        QMessageBox::critical(this,tr("WRONG"),tr("You can only buy for yourself."));
        return;
    }
    QByteArray bytePwd = Pwd.toLatin1(); //trasnform Password for safety.
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    Pwd = bytePwdMd5.toHex();

    QString sql = QString("SELECT * FROM `user` WHERE ID='%1' AND `password`='%2'").arg(UserID).arg(Pwd);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){//账户存在，执行购买操作
        query->clear();
        if(query->exec("BEGIN;")){
            bool sql_ok= true;
            //                          "CALL balanceRefresh('%1',%2); "
            //上悲观锁，避免两个事务同时修改时本应剩余票数加2，但实际上只是加1.（存储过程里）
            QString sql = QString(
                          "CALL TicketsRefundLeftNumRefresh('%1','%2',%3,%4,%5);"
                          "CALL TicketsRefundInsertion('%6',%7);").arg(this->flightID).arg(this->dep_datetime)
                    .arg(this->order_start).arg(this->order_end).arg(this->classType)
                    .arg(this->ticketID)  //.arg(this->UserID).arg(this->newBalance).
                    .arg(this->refundMoney);
            QStringList sqlList = sql.split(";",QString::SkipEmptyParts);
            for (int i=0; i<sqlList.count() && sql_ok; i++)
            {
                qDebug()<<sqlList[i]<<endl;
                sql_ok &= query->exec(sqlList[i]);
            }
            if(sql_ok){
                query->exec("COMMIT;");
                if(FromOrder=="0")
                    QMessageBox::information(this,tr("Hint:"),tr("Refund successfully"));
                else
                    QMessageBox::information(this,tr("Hint:"),tr("You have finished your rebooking."));
                this->close();
            }
            if(!sql_ok){//上述退票过程出现问题，更新撤回
                query->exec("ROLLBACK;");
                acct->setMoney(acct->getMoney()-refundMoney);
                QMessageBox::critical(0, "Error", query->lastError().text());
            }
        }
    }
}
