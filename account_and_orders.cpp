#include "account_and_orders.h"
#include "ui_account_and_orders.h"
#include "recharge.h"
#include "flight_inquiry.h"
#include <QDebug>
#include <QWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QFile>

//构造对象时，必须要给参数：ID 和 Pwd
//此处传入的Pwd是经过MD5转换后的密码，与数据库中所存储的密码对应
extern account_and_orders * acct;

account_and_orders::account_and_orders(QWidget *parent,QString ID,QString Pwd) : QWidget(parent), ui(new Ui::account_and_orders)
{
    ui->setupUi(this);
    QFile styleFile(":/style.qss");//使用qss文件
    styleFile.open(QIODevice::ReadOnly);//只读
    QString setStyleSheet(styleFile.readAll());//读取所有的
    ui->coming_tableView->setStyleSheet(setStyleSheet);//设置样式


    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    //根据登录界面给定的有效用户信息初始化账户界面

    this->UserID=ID;
    this->Password=Pwd;

    QString sql_user = QString("SELECT * FROM user WHERE ID = '%1' AND password = '%2'").arg(ID).arg(Pwd);
    QSqlQuery * query_user = new QSqlQuery();
    query_user->exec(sql_user);
    QString money_string = QString("");
    int money = 0;
    int membership = 0;
    QString name = QString("");
    if(query_user->next()){
        money_string = query_user->value(3).toString();
        money = query_user->value(3).toInt();
        membership = query_user->value(2).toInt();
        name = query_user->value(1).toString();
    }

    this->Name = name;
    this->Money = money;
    this->Membership = membership;

    QString greeting = QString("Hello, %1").arg(this->Name);
    ui->User_label->setText(greeting);
    ui->account_label->setText(tr("Account Balance: ") + money_string + tr("￥"));

    if(membership == 1){
        ui->memInfo_label->setText(tr("Membership: √"));
        ui->Buymem_pushButton->setDisabled(true);
        //成为会员之后不需要再购买会员
    }
    else{
        ui->memInfo_label->setText(tr("Membership: ×"));
    }

    ui->Buymem_pushButton->setText(tr("Pay for Membership (100￥)"));
    ui->Recharge_pushButton->setText(tr("Recharge"));
    ui->Refresh_pushButton->setText(tr("Refresh"));
    ui->buyticket_pushButton->setText("Buy Tickets");

    ui->orders->setTabText(0,tr("Finished Orders"));
    ui->orders->setTabText(1,tr("Coming Orders"));
    ui->orders->setTabText(2,tr("Cancelled Orders"));

    ui->Refresh_pushButton->setText(tr("Recharge"));
    ui->Buymem_pushButton->setText(tr("Pay for Membership (100￥)"));
    ui->buyticket_pushButton->setText(tr("Pay for Flights"));
    ui->Refresh_pushButton->setText(tr("Refresh"));

    this->on_Refresh_pushButton_clicked();

}

account_and_orders::~account_and_orders()
{
    delete ui;
}

void account_and_orders::setMoney(int Money)
{
    this->Money = Money;
}

int account_and_orders::getMoney()
{
    return this->Money;
}

QString account_and_orders::getUserID()
{
    return this->UserID;
}

void account_and_orders::BalanceRefresh()
{
    QString money_string = QString("%1").arg(this->Money);
    ui->account_label->setText(tr("Account Balance: ") + money_string + tr("￥"));
}

void account_and_orders::on_Buymem_pushButton_clicked()
{
    if (this->Money < 100){
        QMessageBox::information(this,tr("Hint"),tr("Sorry, your balance is not enough."));
        return;
    }

    int money = this->Money - 100;

    //更新数据库中的信息
    QString sql_user = QString("UPDATE user "
                               "SET "
                               "membership = 1, "
                               "account = %1 "
                               "WHERE ID = '%2' ").arg(money).arg(this->UserID);
    QSqlQuery * query_user = new QSqlQuery();

    qDebug()<<sql_user <<endl;
    //更新界面
    bool ok = query_user->exec(sql_user);
    if(ok){
        this->Money -= 100;  //扣除会员费
        this->BalanceRefresh();
        ui->memInfo_label->setText(tr("Membership: √"));
        ui->Buymem_pushButton->setDisabled(true);

        QMessageBox::information(this,tr("Hint:"),tr("Your payment has been done."));
    }
    else{
        QMessageBox::information(this,tr("Hint:"),tr("Something wrong with your payment."));
    }


}

void account_and_orders::on_Recharge_pushButton_clicked()
{
    recharge * rec = new recharge();
    rec->show();
}

void account_and_orders::on_Refresh_pushButton_clicked()
{
    this->BalanceRefresh();
    QString ID = this->getUserID();

    QString sql_finished = QString("CALL finished_orders('%1')").arg(ID);
    qDebug()<<sql_finished<<endl;
    FinishedOrderModel *model_finished = new FinishedOrderModel();
    model_finished->setQuery(sql_finished);
    model_finished->setHeaderData(0,Qt::Horizontal, QString::fromUtf8(tr("Ticket ID").toUtf8()));
    model_finished->setHeaderData(1,Qt::Horizontal, QString::fromUtf8(tr("Flight ID").toUtf8()));
    model_finished->setHeaderData(2,Qt::Horizontal, QString::fromUtf8(tr("Departure Time").toUtf8()));
    model_finished->setHeaderData(3,Qt::Horizontal, QString::fromUtf8(tr("Departure Airport").toUtf8()));
    model_finished->setHeaderData(4,Qt::Horizontal, QString::fromUtf8(tr("Arrival Airport").toUtf8()));
    model_finished->setHeaderData(5,Qt::Horizontal, QString::fromUtf8(tr("Class").toUtf8()));
    model_finished->setHeaderData(6,Qt::Horizontal, QString::fromUtf8(tr("Actual Payment").toUtf8()));
    ui->finished_tableView->setModel(model_finished);
    ui->finished_tableView->resizeColumnsToContents();

    QString sql_cancelled = QString("CALL cancelled_orders('%1')").arg(ID);
    qDebug()<<sql_cancelled<<endl;
    CancelledOrderModel *model_cancelled = new CancelledOrderModel();
    model_cancelled->setQuery(sql_cancelled);
    model_cancelled->setHeaderData(0,Qt::Horizontal, QString::fromUtf8(tr("Ticket ID").toUtf8()));
    model_cancelled->setHeaderData(1,Qt::Horizontal, QString::fromUtf8(tr("Flight ID").toUtf8()));
    model_cancelled->setHeaderData(2,Qt::Horizontal, QString::fromUtf8(tr("Departure Time").toUtf8()));
    model_cancelled->setHeaderData(3,Qt::Horizontal, QString::fromUtf8(tr("Departure Airport").toUtf8()));
    model_cancelled->setHeaderData(4,Qt::Horizontal, QString::fromUtf8(tr("Arrival Airport").toUtf8()));
    model_cancelled->setHeaderData(5,Qt::Horizontal, QString::fromUtf8(tr("Class").toUtf8()));
    model_cancelled->setHeaderData(6,Qt::Horizontal, QString::fromUtf8(tr("Actual Payment").toUtf8()));
    ui->cancelled_tableView->setModel(model_cancelled);
    ui->cancelled_tableView->resizeColumnsToContents();

    QString sql_coming = QString("CALL coming_orders('%1')").arg(ID);
    qDebug()<<sql_coming<<endl;
    ComingOrderModel *model_coming = new ComingOrderModel();
    model_coming->setQuery(sql_coming);
    model_coming->insertColumn(7); // 插入退票列
    model_coming->setHeaderData(7,Qt::Horizontal,QString::fromUtf8(tr("Refund").toUtf8()));
    model_coming->insertColumn(8); // 插入值机列
    model_coming->setHeaderData(8,Qt::Horizontal,QString::fromUtf8(tr("Check In").toUtf8()));
    model_coming->setHeaderData(0,Qt::Horizontal, QString::fromUtf8(tr("Ticket ID").toUtf8()));
    model_coming->setHeaderData(1,Qt::Horizontal, QString::fromUtf8(tr("Flight ID").toUtf8()));
    model_coming->setHeaderData(2,Qt::Horizontal, QString::fromUtf8(tr("Departure Time").toUtf8()));
    model_coming->setHeaderData(3,Qt::Horizontal, QString::fromUtf8(tr("Departure Airport").toUtf8()));
    model_coming->setHeaderData(4,Qt::Horizontal, QString::fromUtf8(tr("Arrival Airport").toUtf8()));
    model_coming->setHeaderData(5,Qt::Horizontal, QString::fromUtf8(tr("Class").toUtf8()));
    model_coming->setHeaderData(6,Qt::Horizontal, QString::fromUtf8(tr("Actual Payment").toUtf8()));
    ui->coming_tableView->setModel(model_coming);
    ui->coming_tableView->resizeColumnsToContents();

}


void account_and_orders::on_coming_tableView_clicked(const QModelIndex &index)
{
    if (index.isValid()&&index.column()==7){ //点击退票键，检查退票标准是否成立
        // 如果已值机或者未值机但退票时间Now()距离起飞时间不到3小时，则不能退票；
        // 如果还未值机且时间还未到起飞前3小时，可以退票
        // 根据起飞时间判断返回票价
        qDebug()<<"你刚刚点击了退票按钮"<<endl; //若退票标准成立，则执行退票操作
    }

    if (index.isValid()&&index.column()==8){ //点击值机，进行值机操作
        //
        qDebug()<<"你刚刚点击了值机按钮"<<endl; //选择座位等()
    }

}

//void mainplatformwindow::on_tableView_3_clicked(const QModelIndex &index)
//{
//      if (index.isValid()&&index.column()==8){
//          int row = index.row();
//          QAbstractItemModel* model = ui->tableView_3->model();
//          QString flight_id = model->data(model->index(row,0)).toString();
//          stop_over = new stopover(nullptr,flight_id);
//          stop_over->show();
//      }
//      else if(index.isValid()&&index.column()==10){
//          int row = index.row();
//          QAbstractItemModel* model = ui->tableView_3->model();
//          QString flight_id = model->data(model->index(row,0)).toString();
//          if(QSqlDatabase::database().transaction()){
//              QSqlQuery query;
//              query.exec(tr("delete from airline where flight_id = \'")+flight_id+"\'");
//              query.exec(tr("delete from flight where flight_id = \'")+flight_id+"\'");
//              query.exec(tr("delete from seat where flight_id = \'")+flight_id+"\'");
//              if(!QSqlDatabase::database().commit()){
//                  qDebug()<<QSqlDatabase::database().lastError();
//                  if(!QSqlDatabase::database().rollback()){
//                      qDebug()<<QSqlDatabase::database().lastError();
//                  }

//              }
//              else{
//                  flightRefresh();
//              }
//          }

//      }
//      else if(index.isValid()&&index.column()==9){
//          int row = index.row();
//          QAbstractItemModel* model = ui->tableView_3->model();
//          QString flight_id = model->data(model->index(row,0)).toString();
//          QString schedule = model->data(model->index(row,1)).toString();
//          QString plane_type = model->data(model->index(row,2)).toString();
//          QString company_id = model->data(model->index(row,7)).toString();
//          modification_flight = new modflight(nullptr,flight_id,schedule,plane_type,company_id);
//          modification_flight->show();
//      }

//}



QVariant FinishedOrderModel::data(const QModelIndex &item, int role) const{
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::DisplayRole){
        if(item.column()==5){
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("Business")));
            else
                return QVariant::fromValue(QString(tr("Economy")));
        }
    }
    return value;
}


QVariant ComingOrderModel::data(const QModelIndex &item, int role) const{
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::BackgroundColorRole){
        if(item.column()==7)
            return QVariant::fromValue(QColor(225,225,225));
        else if(item.column()==8)
            return QVariant::fromValue(QColor(225,225,225));//颜色特殊表示，起到类似按钮的作用
    }
    if (role == Qt::DisplayRole){
        if(item.column()==5){
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("Business")));
            else
                return QVariant::fromValue(QString(tr("Economy")));
        }
        if(item.column()==7)
            return QVariant::fromValue(tr("Refund"));
        else if(item.column()==8)
            return QVariant::fromValue(tr("Check In"));
        }
        return value;
}

QVariant CancelledOrderModel::data(const QModelIndex &item, int role) const{
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::DisplayRole){
        if(item.column()==5){
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("Business")));
            else
                return QVariant::fromValue(QString(tr("Economy")));
        }
    }
    return value;
}



void account_and_orders::on_buyticket_pushButton_clicked()
{
    flight_inquiry * flight = new flight_inquiry(nullptr,this->UserID,this->Password,this->Name);
    flight->show();
    //ui->buyticket_pushButton->setDisabled(true); //不允许成功打开查询页面后重复查询.
}
