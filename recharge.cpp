#include "recharge.h"
#include "ui_recharge.h"
#include "account_and_orders.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QDebug>
#include <QLayout>

extern account_and_orders *acct ;

recharge::recharge(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::recharge)
{
    ui->setupUi(this);

    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    ui->ID_label->setText(tr("ID:"));
    ui->Pwd_label->setText(tr("Password:"));
    ui->Amount_label->setText(tr("Amount:"));

    ui->ID_lineEdit->setPlaceholderText(tr("Please enter your ID."));
    ui->ID_lineEdit->setEchoMode(QLineEdit::Normal);

    ui->Pwd_lineEdit->setPlaceholderText(tr("Please enter your Password."));
    ui->Pwd_lineEdit->setEchoMode(QLineEdit::Password);

    ui->Amount_lineEdit->setPlaceholderText(tr("Please enter the amount."));
    ui->Amount_lineEdit->setEchoMode(QLineEdit::Normal);

    ui->Ok_pushButton->setText(tr("OK"));
    ui->Back_pushButton->setText(tr("Cancel"));

    ui->Hint_label->setText(tr("Recharge Interface:"));
}

recharge::~recharge()
{
    delete ui;
}

//鉴于是用户窗口的子窗口，所以就不设置返回，而是设置为取消
void recharge::on_Back_pushButton_clicked()
{
    this->close();
}

void recharge::on_Ok_pushButton_clicked()
{
    QString ID = ui->ID_lineEdit->text();
    QString Pwd = ui->Pwd_lineEdit->text();
    QString amount_str = ui->Amount_lineEdit->text();
    float amount = amount_str.toFloat();

    //将密码进行md5转换，以获得数据库中存储的密码；
    if (ID==""||Pwd==""||amount_str==""){
        QMessageBox::critical(this,"critical","incomplete input.");
        return;
    }

    QByteArray bytePwd = Pwd.toLatin1(); //trasnform Password for safety.
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    Pwd = bytePwdMd5.toHex();
    qDebug()<<Pwd<<endl;

    if(ID != acct->getUserID()){
        QMessageBox::critical(this,"Hint","You can't recharge another account!");
        return;
    }

    QString sql_check = QString("SELECT * FROM user where ID = '%1' "
                                "AND password = '%2'").arg(ID).arg(Pwd);
    QSqlQuery *query_check = new QSqlQuery();
    query_check->exec(sql_check);

    if(!query_check->next()){
        QMessageBox::critical(this,"Hint","Not a valid ID and Password.");
        return;
    }


    amount += acct->getMoney();

    QString sql = QString("UPDATE user "
                          "SET"
                          "  account = %1 "
                          "WHERE ID = '%2' AND password = '%3'").arg(amount).arg(ID).arg(Pwd);

    qDebug()<<sql<<endl;

    QSqlQuery *query = new QSqlQuery();
    bool ok = query->exec(sql);

    if (ok){
        acct->setMoney(amount);
        acct->BalanceRefresh(); //用于更新账户界面的余额信息
        QMessageBox::information(this,"Hint","Recharge successfull.");
        this->close();
    }else{
        QMessageBox::critical(this,"Hint","Something wrong with your recharging.");
    }

}
