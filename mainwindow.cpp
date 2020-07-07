#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainclientwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include<QMessageBox>
#include<QTranslator>
QTranslator translator;
//QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
QSqlDatabase db;
MainClientWindow *w;
MainWindow *startwindow;
extern QSettings settings;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    switch(settings.value("Client/Langcase",2).toInt()) {
    case 0: {
        break;
    }

    case 1: {
        QString langdir = ":/client_zh_CN.qm";
        translator.load(langdir);
        qApp->installTranslator(&translator);

        ui->retranslateUi(this);
        break;
    }

    default : {
        QString langdir = ":/client_" + QLocale::system().name() + ".qm";
        translator.load(langdir);
        qApp->installTranslator(&translator);

        ui->retranslateUi(this);
        settings.setValue("Client/Langcase", 2);
    }
    }
    startwindow=this;
    ui->retranslateUi(this);
    ui->checkBox->setChecked(true);
    on_commandLinkButton_clicked();
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_commandLinkButton_clicked() {
    db = QSqlDatabase::addDatabase("QMYSQL");
    /* if(ui->checkBox->isChecked()==true){
         ui->editHostname->setText("gz.aloereed.cc,1433\\SQLEXPRESS");
         ui->editDatabase->setText("flights");
         ui->editPassword->setText("sql4021");
         ui->editUsername->setText("lzq");
     }*/
    if(ui->checkBox->isChecked()==true) {
        ui->editHostname->setText("cdb-aaxnskss.cd.tencentcdb.com");
        ui->editDatabase->setText("flight");
        ui->editPassword->setText("sqlfd4021");
        ui->editUsername->setText("root");
        ui->portSpinBox->setValue(10095);
    }
    QString serverIp=ui->editHostname->text();
    QString databaseName=ui->editDatabase->text();
    QString user=ui->editUsername->text();
    QString password=ui->editPassword->text();
    int port = ui->portSpinBox->value();


    if(serverIp==""||databaseName==""||user==""||password=="") {
        QMessageBox::critical(this,"Server is wrong, reset please.","critical");
        return;

    }

    //QSqlDatabase db1 = QSqlDatabase::addDatabase("QODBC");

    db.setHostName(serverIp);
    db.setPort(port);
    db.setDatabaseName(databaseName);
    db.setUserName(user);
    db.setPassword(password);

    /* db.setDatabaseName(QString("DRIVER={SQL SERVER};"
                                  "SERVER=%1;"
                                  "DATABASE=%2;"
                                   "UID=%3;"
                                   "PWD=%4;").arg(serverIp)
                                             .arg(databaseName)
                                             .arg(user)
                                             .arg(password)); */


    if (db.open()) {
        w = new MainClientWindow();
        w->show();
        this->hide();
    } else {
        QMessageBox::critical(this,"Server is wrong, reset please.",db.lastError().text());
        return;
    }
}
