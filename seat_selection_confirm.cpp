#include "seat_selection_confirm.h"
#include "ui_seat_selection_confirm.h"
#include "account_and_orders.h"
#include "seat_selection.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QSqlQuery>

extern account_and_orders * acct;

seat_selection_confirm::seat_selection_confirm(QWidget *parent,seat_selection*parent_true,QString seatID) :
    QWidget(parent),
    ui(new Ui::seat_selection_confirm)
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

    this->parent = parent_true;
    this->seatID = seatID;
}

seat_selection_confirm::~seat_selection_confirm()
{
    delete ui;
}

void seat_selection_confirm::on_pushButton_cancel_clicked()
{
    this->close();
}

void seat_selection_confirm::on_pushButton_confirm_clicked()
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
        //开启一个上锁的事务
        if(query->exec("BEGIN;")){
            bool sql_ok= true;
            sql = QString("CALL SeatsSelection('%1',%2,%3,'%4','%5','%6')")
                    .arg(this->parent->getflightID()).arg(this->parent->getorder_start())
                    .arg(this->parent->getorder_end()).arg(this->parent->getdep_date())
                    .arg(this->seatID).arg(acct->getUserID());
            sql_ok &= query->exec(sql);
            if(sql_ok){
                query->exec("COMMIT");
            }else{  //if(! sql_ok)
                 query->exec("ROLLBACK");
                 QMessageBox::critical(this,tr("WRONG"),tr("Something went wrong. Please try again."));
                 this->parent->close();
                 this->close();
                 return;
            }
        }
        query->clear();
        if(query->exec(sql)){
            sql = QString("SELECT passengerID FROM seat_arrangement WHERE "
                                  "flight_id='%1' AND `order`=%2 AND departure_date='%3' AND seat_id='%4' ")
                    .arg(this->parent->getflightID()).arg(this->parent->getorder_start())
                    .arg(this->parent->getdep_date()).arg(this->seatID);
            query->clear();
            query->exec(sql);
            query->first();
            if(query->value(0).toString() != acct->getUserID()){ //说明位置已经被别人先选择了，提示重新值机
                QMessageBox::information(this,tr("Hint"),tr("Someone has selected this seat ahead of you. Please redo your check in."));
            }else{ //说明值机成功
                QMessageBox::information(this,tr("Hint"),tr("You have selected your seat. Please check it in your order."));
            }
            this->parent->close();
            this->close();
            return;
        }else{ //说明该sql语句没执行成功，重试
            QMessageBox::critical(this,tr("WRONG"),tr("Something went wrong. Please try again."));
            this->parent->close();
            this->close();
            return;
        }
    }else{//账户不存在
        QMessageBox::critical(this,tr("WRONG"),tr("Something went wrong. Please try again."));
        this->parent->close();
        this->close();
        return;
    }
}
