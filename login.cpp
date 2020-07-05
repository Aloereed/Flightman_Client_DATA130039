#include "login.h"
#include "ui_login.h"
#include "registration.h"
#include "mainclientwindow.h"
#include "account_and_orders.h"
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QCryptographicHash>

account_and_orders *acct;

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->UserID->setEchoMode(QLineEdit::Normal);
    ui->UserID->setPlaceholderText(tr("Please Enter Your ID."));

    ui->Password->setEchoMode(QLineEdit::Password);
    ui->Password->setPlaceholderText(tr("Please Enter Your Password."));

    ui->label->setText(tr("ID"));
    ui->label_2->setText(tr("password"));
    ui->label_5->setText(tr("No account yet?"));
    ui->label_6->setText(tr("Back to MainWindow?"));
    ui->label_7->setText(tr("Want to quit?"));
    ui->pushButton->setText(tr("Log In"));
    ui->pushButton_2->setText(tr("Register Now"));
    ui->pushButton_4->setText(tr("Back"));
    ui->pushButton_5->setText(tr("Cancel"));
}

login::~login()
{
    delete ui;
}

//Btn: Register Now
void login::on_pushButton_2_clicked()
{
    registration * regis = new registration();
    QApplication::processEvents();
    regis->show();
    QApplication::processEvents();
    this->close();
}
//Btn: Back
void login::on_pushButton_4_clicked()
{
    MainClientWindow * mainclientwindow = new MainClientWindow();
    QApplication::processEvents();
    mainclientwindow->show();
    QApplication::processEvents();
    this->close();

}
//Btn: Cancel
void login::on_pushButton_5_clicked()
{
    qDebug()<<"You decide to quit."<<endl;
    this->close();
}

void login::on_pushButton_clicked()
{
    QString UserID = ui->UserID->text();
    QString Password = ui->Password->text();

    if (UserID==""||Password==""){
        QMessageBox::critical(this,tr("critical"),tr("incomplete input."));
        return;
    }

    QByteArray bytePwd = Password.toLatin1(); //trasnform Password for safety.
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    Password = bytePwdMd5.toHex();

    QString sql = QString("SELECT * FROM user WHERE ID='%1' "
                          "AND password='%2'").arg(UserID).arg(Password);

    QSqlQuery * query = new QSqlQuery();
    query->exec(sql);

    if(query->next()) {
        //若查询成功，则进入到相应的用户界面
        acct = new account_and_orders(nullptr,UserID,Password);
        acct->show();
        this->close();
    }
    else
    {
    QMessageBox::critical(this,tr("critical"),tr("The account doesn't exist."));
    return;
    }
}
