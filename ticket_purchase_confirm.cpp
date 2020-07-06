#include "ticket_purchase_confirm.h"
#include "ui_ticket_purchase_confirm.h"
#include "account_and_orders.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QCryptographicHash>

extern account_and_orders * acct;
ticket_purchase_confirm::ticket_purchase_confirm(QWidget *parent,Ticket_Purchase *parent_ticket
                                                 ,QString moneyStr,QString price,QString classType) :
    QWidget(parent),
    ui(new Ui::ticket_purchase_confirm)
{
    ui->setupUi(this);

    ui->groupBox->setTitle(tr("Authentication:"));
    ui->label_UserID->setText(tr("User ID :"));
    ui->lineEdit_UserID->setPlaceholderText(tr("Please Enter Your ID."));
    ui->lineEdit_UserID->setEchoMode(QLineEdit::Normal);

    ui->label_Password->setText(tr("Password :"));
    ui->lineEdit_Password->setPlaceholderText(tr("Please Enter Your Password."));
    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);

    ui->pushButton_confirm->setText(tr("Confirm"));
    ui->pushButton_cancel->setText(tr("Cancel"));

    this->parent_ticket = parent_ticket;
    this->moneyStr = moneyStr;
    this->classType = classType;
    this->price = price;

}

ticket_purchase_confirm::~ticket_purchase_confirm()
{
    delete ui;
}

void ticket_purchase_confirm::on_pushButton_cancel_clicked()
{
    this->close();
}

void ticket_purchase_confirm::on_pushButton_confirm_clicked()
{
    QString UserID = ui->lineEdit_UserID->text();
    QString Pwd = ui->lineEdit_Password->text();
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
    QString companyID = this->parent_ticket->get_flightID().mid(0,2);
    if(query->next()){//账户存在，执行购买操作
        this->parent_ticket->Payment(this->parent_ticket->get_ptr_CD(),this->parent_ticket->get_ptr_flightID(),acct->getUserID(),moneyStr,this->price
                                     ,this->parent_ticket->get_flightID(),this->parent_ticket->get_depDate()
                                     ,this->parent_ticket->get_depTime(),this->parent_ticket->get_orderStart(),
                                     this->parent_ticket->get_orderEnd(),this->classType,companyID);
    qDebug()<<"完成支付！"<<endl;
    this->close();

    }else{//说明账户不存在
        QMessageBox::critical(this,tr("WRONG"),tr("Account does not exist. Please re-enter."));
        return;
    }
}
