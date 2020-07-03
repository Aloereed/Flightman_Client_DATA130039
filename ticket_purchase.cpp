#include "ticket_purchase.h"
#include "ui_ticket_purchase.h"
#include "account_and_orders.h"
#include <QDebug>
#include <QSqlQuery>

extern account_and_orders * acct;

Ticket_Purchase::Ticket_Purchase(QWidget *parent, QString dep_date, QString flightID,
                                 QString schedule, QString dep_airportName,
                                 QString dep_city, QString dep_time, QString arv_airportName, QString arv_city, QString arv_time,
                                 QString orderstart,QString orderend) :
    QWidget(parent),
    ui(new Ui::Ticket_Purchase)
{
    ui->setupUi(this);
    ui->Info_groupBox->setTitle(tr("Flights Information:"));
    ui->label_date->setText(dep_date);
    this->depature_date=dep_date;
    ui->label_schedule->setText(schedule);
    this->schedule=schedule;
    ui->label_depCity->setText(dep_city);
    this->departure_city=dep_city;
    ui->label_arvCity->setText(arv_city);
    this->arrival_city=arv_city;
    //自行查询航空公司
    ui->label_company->setText(this->CompanyQuery(flightID));

    ui->label_flightID->setText(flightID);
    this->flightID=flightID;
    ui->label_depTime->setText(dep_time);
    this->departure_time=dep_time;
    ui->label_arvTime->setText(arv_time);
    this->arrival_time=arv_time;
    ui->label_depAirport->setText(dep_airportName);
    this->depature_airportName=dep_airportName;
    ui->label_arvAirport->setText(arv_airportName);
    this->arrival_airportName=arv_airportName;

    this->orderstart=orderstart;
    this->orderend=orderend;

    ui->Price_groupBox->setTitle(tr("Flights Price"));
    ui->radioButton_econ->setText(tr("Economy Class"));
    ui->radioButton_busi->setText(tr("Business Class"));
    ui->radioButton_econ->click(); //默认选择经济舱
    ui->label_type->setText(tr("Class Type"));
    //自行查询余票数
    ui->label_remainingTickets->setText(tr("Tickets Left"));

    //自行查询价格
    ui->label_Price->setText(tr("Price"));
    QString PrimalPrice = this->PrimalPriceQuery(flightID,"1",orderstart,orderend);//经济舱的原价
    QString TotalDiscount = this->DiscountQuery(flightID,dep_date,"1");//经济舱的折扣
    float price = PrimalPrice.toFloat()*TotalDiscount.toFloat();
    QString PriceStr = QString("%1").arg(price);
    ui->label_PriceEcon->setText(PriceStr);

    PrimalPrice = this->PrimalPriceQuery(flightID,"0",orderstart,orderend);//公务舱的原价
    TotalDiscount = this->DiscountQuery(flightID,dep_date,"0");//公务舱的折扣
    price = PrimalPrice.toFloat()*TotalDiscount.toFloat();
    PriceStr = QString("%1").arg(price);
    ui->label_PriceBusi->setText(PriceStr);

    //自行查询折扣
    ui->label_Discount->setText(tr("Discount"));

    int balance = acct->getMoney();
    QString money_string = QString("%1").arg(balance);
    ui->label_Balance->setText(tr("Balance:"));
    ui->label_BalanceMoney->setText(money_string);//设置账户余额

    ui->pushButton->setText(tr("Purchase"));

}


Ticket_Purchase::~Ticket_Purchase()
{
    delete ui;
}

QString Ticket_Purchase::PrimalPriceQuery(QString flightID, QString classType, QString order_start, QString order_end)
{
    QString sql = QString("SELECT price FROM price WHERE flight_id='%1' AND start_id=%2 AND end_id=%3 "
                          "AND class=%4").arg(flightID).arg(order_start).arg(order_end).arg(classType);
    qDebug()<<sql<<endl;
    QString PrimalPrice = "-1"; //如果查询不存在，默认为-1，表示价格无穷大，消费者无法购买
    QSqlQuery * query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){
        PrimalPrice = query->value(0).toString();
    }
    return PrimalPrice;
}

QString Ticket_Purchase::DiscountQuery(QString flightID, QString depDate, QString classType)
{
    QString sql = QString("SELECT discount FROM flight_arrangement WHERE departure_date='%1' "
                          "AND flight_id='%2'").arg(depDate).arg(flightID);
    qDebug()<<sql<<endl;
    //查询日期折扣
    float discount = 0.00; //如果查询不存在，默认为0，折扣为0，价格免费，消费者仍无法购买
    QSqlQuery * query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){
        discount = query->value(0).toFloat();
    }
    //查询会员折扣
    if(acct->getMembership()==1){ //说明此人是会员，可以去会员表中查询相应舱位的折扣
        sql = QString("SELECT discount FROM membership_discount WHERE class=%1").arg(classType);
        query->clear();
        query->exec(sql);
        if(query->next()){
            discount *= query->value(0).toFloat();
        }
    }
    QString PriceString = QString("%1").arg(discount);
    return PriceString;
}

QString Ticket_Purchase::CompanyQuery(QString flightID)
{
    QString sql = QString("SELECT company_name FROM flight INNER JOIN company ON flight.company_id=company.company_id "
                          "WHERE flight_id='%1'").arg(flightID);
    qDebug()<<sql<<endl;
    QString company = QString(tr("国内航空")); //默认是国内航空
    QSqlQuery * query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){
        company = query->value(0).toString();
    }
    return company;
}

//Btn: purchase
void Ticket_Purchase::on_pushButton_clicked()
{
    qDebug()<<"你刚刚点击了购买按钮！"<<endl;
    //检查此时能否购票，即查询一遍有无余票剩余
    //若可以购票，则相应的有如下的结果：（如下结果按照事务来处理！）
    //用户自身的余额要扣除相应对等的数额（如果数额不够，则提示“余额不足，无法购票”）
    //ticket里面增加相应记录，ticket_purchase里面也要增加相应记录
    //余票数目减一，也即是相应航程覆盖的区间的剩余票数都减一
    //由于购票只算人头，那么值机的部分与此独立
}
