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
#include<QStackedWidget>
#include<QSettings>
extern QStackedWidget* mainstack;
account_and_orders *acct;
extern QSettings settings;
extern MainClientWindow *w;
login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login) {
    ui->setupUi(this);
    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    ui->UserID->setEchoMode(QLineEdit::Normal);
    ui->UserID->setPlaceholderText(tr("Please Enter Your ID."));

    ui->Password->setEchoMode(QLineEdit::Password);
    ui->Password->setPlaceholderText(tr("Please Enter Your Password."));
    ui->UserID->setText(settings.value("User/UserID","").toString());

    if(settings.value("User/isRem",false).toBool()) {
        ui->checkBox_2->setChecked(settings.value("User/isAuto").toBool());
        ui->Password->setText(settings.value("User/UserPWD").toString());
        ui->checkBox->setChecked(true);
    }
    ui->label->setText(tr("ID"));
    ui->label_2->setText(tr("Password"));
    ui->label_5->setText(tr("No account yet?"));
    ui->label_6->setText(tr("Back to MainWindow?"));
    ui->label_7->setText(tr("Want to quit?"));
    ui->pushButton->setText(tr("Log In"));
    ui->pushButton_2->setText(tr("Register Now"));
    ui->pushButton_4->setText(tr("Back"));
    ui->pushButton_5->setText(tr("Cancel"));
    //if(ui->checkBox_2->isChecked())
    //   on_pushButton_clicked();
}

login::~login() {
    delete ui;
}

//Btn: Register Now
void login::on_pushButton_2_clicked() {
    registration * regis = new registration();
    QApplication::processEvents();
    regis->show();
    QApplication::processEvents();
}
//Btn: Back
void login::on_pushButton_4_clicked() {
    MainClientWindow * mainclientwindow = new MainClientWindow();
    QApplication::processEvents();
    mainclientwindow->show();
    QApplication::processEvents();
    this->close();

}
//Btn: Cancel
void login::on_pushButton_5_clicked() {
    qDebug()<<"You decide to quit."<<endl;
    this->close();
}

void login::on_pushButton_clicked() {
    QString UserID = ui->UserID->text();
    QString Password = ui->Password->text();

    if (UserID==""||Password=="") {
        QMessageBox::critical(this,tr("critical"),tr("incomplete input."));
        return;
    }

    QByteArray bytePwd = Password.toLatin1(); //trasnform Password for safety.
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    QString PasswordBack=Password;
    Password = bytePwdMd5.toHex();

    QString sql = QString("SELECT * FROM user WHERE ID='%1' "
                          "AND password='%2'").arg(UserID).arg(Password);

    QSqlQuery * query = new QSqlQuery();
    query->exec(sql);

    if(query->next()) {

        settings.setValue("User/UserID",UserID);
        settings.setValue("User/isRem",ui->checkBox->isChecked());
        settings.setValue("User/isAuto",ui->checkBox_2->isChecked());
        //若查询成功，则进入到相应的用户界面
        if(ui->checkBox->isChecked()) {

            settings.setValue("User/UserPWD",PasswordBack);

        }
        settings.sync();
        acct = new account_and_orders(nullptr,UserID,Password);
        mainstack->addWidget(acct);
        mainstack->setCurrentWidget(acct);

        qDebug()<<"test";
        this->close();

    } else {
        QMessageBox::critical(this,tr("critical"),tr("The account doesn't exist."));
        return;
    }
}
void login::hideforhome() {
    ui->pushButton_4->hide();
    ui->pushButton_5->hide();
    ui->label_6->hide();
    ui->label_7->hide();
}

void login::on_checkBox_2_clicked() {
    if(ui->checkBox_2->isChecked())
        ui->checkBox->setChecked(true);
}

void login::on_checkBox_clicked() {
    if(ui->checkBox_2->isChecked())
        ui->checkBox->setChecked(true);
}
bool login::isAuto() {
    return ui->checkBox_2->isChecked();
}
