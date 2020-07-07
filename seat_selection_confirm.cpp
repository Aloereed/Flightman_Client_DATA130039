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
        sql = QString("CALL SeatsSelection('%1',%2,%3,'%4','%5','%6')")
                .arg(this->parent->getflightID()).arg(this->parent->getorder_start())
                .arg(this->parent->getorder_end()).arg(this->parent->getdep_date())
                .arg(this->seatID).arg(acct->getUserID());
        query->clear();
        if(query->exec(sql)){
            QMessageBox::information(this,tr("Hint"),tr("You have selected your seat. Please check it in your order."));
            this->parent->close();
            this->close();
            return;
        }else{
            QMessageBox::critical(this,tr("WRONG"),tr("Something went wrong. Please try again."));
            return;
        }
    }else{
        QMessageBox::critical(this,tr("WRONG"),tr("Something went wrong. Please try again."));
        return;
    }
}
