#include "messagebox.h"
#include "ui_messagebox.h"
#include "account_and_orders.h"
#include <QSqlQuery>
#include <QSqlQueryModel>

extern account_and_orders * acct;

messagebox::messagebox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::messagebox)
{
    ui->setupUi(this);
    ui->groupBox->setTitle(tr("Some information you need to notice:"));
    ui->pushButton_close->setText(tr("Close"));
    ui->pushButton_refresh->setText(tr("Refresh"));
    this->on_pushButton_refresh_clicked();
    ui->tableView_messages->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(sql);
    model->setHeaderData(0,Qt::Horizontal, QString::fromUtf8(tr("Release Time").toUtf8()));
    model->setHeaderData(1,Qt::Horizontal, QString::fromUtf8(tr("Announcement").toUtf8()));
    ui->tableView_messages->setModel(model);
    ui->tableView_messages->resizeColumnsToContents();
}
