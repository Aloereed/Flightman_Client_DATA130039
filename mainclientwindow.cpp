#include "mainclientwindow.h"
#include "ui_mainclientwindow.h"
#include "mainwindow.h"
#include "flight_inquiry.h"
#include "login.h"
#include "account_and_orders.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include<QMessageBox>
#include<QStackedWidget>
QStackedWidget* mainstack;
extern QSqlDatabase db;
flight_inquiry * flt_in;
login *lgin;
extern account_and_orders *acct;
extern MainWindow *startwindow;
MainClientWindow::MainClientWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainClientWindow) {
    ui->setupUi(this);

    mainstack=ui->stackedWidget;
    ui->widget->setMaximumHeight(this->height()/8);
    ui->pushButton_3->setMaximumHeight(this->height()/11);
    ui->pushButton_3->setMaximumWidth(this->height()/11);
    ui->pushButton_3->setIconSize(ui->pushButton_3->rect().size());
    ui->pushButton->setIconSize(ui->pushButton->rect().size());
    ui->pushButton_2->setIconSize(ui->pushButton_2->rect().size());
    flt_in=new flight_inquiry(nullptr,"","","");
    lgin = new login();
    ui->stackedWidget->addWidget(flt_in);
    ui->stackedWidget->addWidget(lgin);
    ui->stackedWidget->setCurrentWidget(flt_in);
    //lgin->hide();
    //QMessageBox::information(this,db.databaseName(),"Connected.");
}

MainClientWindow::~MainClientWindow() {
    delete ui;
}

void MainClientWindow::on_pushButton_clicked() {
    /*
     * flight_inquiry *flit_in = new flight_inquiry();
    flit_in->show();
    QApplication::processEvents();
    this->hide();
    */
    //flight_inquiry * flt_in = new flight_inquiry(nullptr,"","","");
    ui->stackedWidget->setCurrentWidget(flt_in);
    QApplication::processEvents();
    //this->close();

}

void MainClientWindow::on_pushButton_2_clicked() {
    //login *lgin = new login();
    if(acct==NULL)
        ui->stackedWidget->setCurrentWidget(lgin);
    else
        ui->stackedWidget->setCurrentWidget(acct);
    QApplication::processEvents();
    //this->close();
}

void MainClientWindow::on_pushButton_3_clicked() {
    qDebug()<<"You decide to quit."<<endl;
    QApplication::quit();
}

void MainClientWindow::on_toolButton_clicked() {

}
