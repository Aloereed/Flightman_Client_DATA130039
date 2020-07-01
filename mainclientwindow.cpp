#include "mainclientwindow.h"
#include "ui_mainclientwindow.h"
#include "mainwindow.h"
#include "flight_inquiry.h"
#include "login.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include<QMessageBox>
extern QSqlDatabase db;
MainClientWindow::MainClientWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainClientWindow)
{
    ui->setupUi(this);
    //QMessageBox::information(this,db.databaseName(),"Connected.");
}

MainClientWindow::~MainClientWindow()
{
    delete ui;
}

void MainClientWindow::on_pushButton_clicked()
{
    /*
     * flight_inquiry *flit_in = new flight_inquiry();
    flit_in->show();
    QApplication::processEvents();
    this->hide();
    */
    flight_inquiry * flt_in = new flight_inquiry();
    flt_in->show();
    QApplication::processEvents();
    this->close();

}

void MainClientWindow::on_pushButton_2_clicked()
{
    login *lgin = new login();
    lgin->show();
    QApplication::processEvents();
    this->close();
}

void MainClientWindow::on_pushButton_3_clicked()
{
    qDebug()<<"You decide to quit."<<endl;
    QApplication::quit();
}
