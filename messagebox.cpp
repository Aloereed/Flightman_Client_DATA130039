#include "messagebox.h"
#include "ui_messagebox.h"
#include "account_and_orders.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QScroller>

extern account_and_orders * acct;

messagebox::messagebox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::messagebox)
{
    ui->setupUi(this);
    ui->groupBox_individual->setTitle(tr("Personal Information:"));
    ui->groupBox_all->setTitle(tr("Collective Information:"));
    ui->pushButton_close->setText(tr("Close"));
    ui->pushButton_refresh->setText(tr("Refresh"));
    this->on_pushButton_refresh_clicked();
    ui->tableView_all->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QScroller::grabGesture(ui->tableView_all, QScroller::TouchGesture);
    ui->tableView_individual->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QScroller::grabGesture(ui->tableView_individual, QScroller::TouchGesture);
    ui->information->setTabText(0,tr("Collective"));
    ui->information->setTabText(1,tr("Personal"));
}

messagebox::~messagebox()
{
    delete ui;
}

void messagebox::on_pushButton_close_clicked()
{
    this->close();
}

void messagebox::on_pushButton_refresh_clicked()
{
    QString sql = QString("SELECT time,text FROM announcement WHERE userID = '%1'").arg(acct->getUserID());
    QSqlQueryModel *model_individual = new QSqlQueryModel();
    model_individual->setQuery(sql);
    model_individual->setHeaderData(0,Qt::Horizontal, QString::fromUtf8(tr("Release Time").toUtf8()));
    model_individual->setHeaderData(1,Qt::Horizontal, QString::fromUtf8(tr("Announcement").toUtf8()));
    ui->tableView_individual->setModel(model_individual);
    ui->tableView_individual->resizeColumnsToContents();

    sql = QString("SELECT time,text FROM announcement WHERE userID = '000000000000000000'");
    QSqlQueryModel *model_all = new QSqlQueryModel();
    model_all->setQuery(sql);
    model_all->setHeaderData(0,Qt::Horizontal, QString::fromUtf8(tr("Release Time").toUtf8()));
    model_all->setHeaderData(1,Qt::Horizontal, QString::fromUtf8(tr("Announcement").toUtf8()));
    ui->tableView_all->setModel(model_all);
    ui->tableView_all->resizeColumnsToContents();
}
