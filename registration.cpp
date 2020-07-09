#include "registration.h"
#include "ui_registration.h"
#include "login.h"
#include <QSqlError>
#include <QMessageBox>
#include <QWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QDebug>

extern QSqlDatabase db;

registration::registration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registration)
{
    ui->setupUi(this);
    qDebug()<<db.transaction()<<endl;
    ui->lineEdit->setEchoMode(QLineEdit::Normal);//ID line
    ui->lineEdit->setPlaceholderText(tr("Please Enter Your ID."));

    ui->lineEdit_2->setEchoMode(QLineEdit::Normal);//Name line
    ui->lineEdit_2->setPlaceholderText(tr("Please Enter Your Name."));

    ui->lineEdit_3->setEchoMode(QLineEdit::PasswordEchoOnEdit);//Password line
    ui->lineEdit_3->setPlaceholderText(tr("Please Enter Your Password."));

}

registration::~registration()
{
    delete ui;
}

//Btn: Back
void registration::on_pushButton_clicked()
{

    this->hide();
}

void registration::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->button(QDialogButtonBox::Ok) == button)
    {
        if(ui->lineEdit->text()==""||ui->lineEdit_2->text()==""||ui->lineEdit_3->text()==""){
            QMessageBox::information(this,tr("hint:"),tr("failure"));
            return;
        }
        QByteArray bytePwd = ui->lineEdit_3->text().toLatin1(); //Password line
        QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
        QString strPwdMd5 = bytePwdMd5.toHex();
        my_user tran;
        tran.ID = ui->lineEdit->text(); //ID line
        tran.name = ui->lineEdit_2->text(); //Name line
        //tran.membership = QString(ui->checkVIPBox->isChecked()?"1":"0"); 此处只需要 ID name和password
        tran.passwordmd5=strPwdMd5;

        //检查有无重复的ID;
        QString check = QString("SELECT ID from user where ID = '%1' ").arg(tran.ID);
        QSqlQuery sql_check;
        sql_check.exec(check);
        if (sql_check.next()){
            QMessageBox::information(this,tr("hint:"),tr("ID existed."));
            return;
        }


        QProgressDialog dialog(tr("Adding"),tr("cancel"), 0, 30000, this);
        dialog.setWindowTitle(tr("process"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for(int k = 0; k < 30000; k++)
        {
            dialog.setValue(k);
            QCoreApplication::processEvents();
            if(dialog.wasCanceled())
            {
                break;
            }
        }
        dialog.setValue(30000);

        QString sql; //默认非会员状态，且初始账户余额为0
        sql = QString("INSERT INTO user (ID,name,membership,account,password) "
                      "VALUES('%1','%2',%3,%4,'%5')")
                .arg(tran.ID).arg(tran.name).arg(0).arg(0).arg(tran.passwordmd5);
        QSqlQuery query;
        bool ok = query.exec(sql);
        if(ok){
            QMessageBox::information(this,tr("hint:"),tr("account created successfully"));
        }
        else{
            QMessageBox::information(this,tr("hint:"),tr("failure"));
        }
        this->close();
    }
    else if(ui->buttonBox->button(QDialogButtonBox::Cancel) == button)
    {
        this->close();
        QProgressDialog dialog(tr("Returning to the mainwindow"),tr("cancel"), 0, 3000, this);
        dialog.setWindowTitle(tr("process"));
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();
        for(int k = 0; k < 3000; k++)
        {
            dialog.setValue(k);
            QCoreApplication::processEvents();
            if(dialog.wasCanceled())
            {
                break;
            }
        }
        dialog.setValue(3000);
     }

}
