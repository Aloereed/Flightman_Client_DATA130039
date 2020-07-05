#include "ticket_purchase.h"
#include "ui_ticket_purchase.h"
#include "account_and_orders.h"
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>

extern account_and_orders * acct;

Ticket_Purchase::Ticket_Purchase(QWidget *parent,flight_inquiry_citys_and_date *parent1,flight_inquiry_flightID *parent2,
                                 QString dep_date, QString flightID,
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
    this->ptr_CD = parent1;
    this->ptr_flightID = parent2;

    ui->Price_groupBox->setTitle(tr("Flights Price"));
    ui->radioButton_econ->setText(tr("Economy Class"));
    ui->radioButton_busi->setText(tr("Business Class"));
    ui->radioButton_econ->click(); //默认选择经济舱
    ui->label_type->setText(tr("Class Type"));
    //自行查询余票数
    ui->label_remainingTickets->setText(tr("Tickets Left"));
    this->TicketsLeftRefresh();
//    ui->label_remainEcon->setText(this->TicketsLeftQuery(flightID,dep_date,orderstart,orderend,"1"));//显示经济舱余票
//    ui->label_reaminingBusi->setText(this->TicketsLeftQuery(flightID,dep_date,orderstart,orderend,"0"));//显示公务舱余票

    //自行查询价格与折扣
    ui->label_Price->setText(tr("Price"));
    ui->label_Discount->setText(tr("Discount"));
    QString PrimalPrice = this->PrimalPriceQuery(flightID,"1",orderstart,orderend);//经济舱的原价
    QString TotalDiscount = this->DiscountQuery(flightID,dep_date,"1");//经济舱的折扣
    ui->label_DiscountEcon->setText(TotalDiscount);
    float price = PrimalPrice.toFloat()*TotalDiscount.toFloat();
    QString PriceStr = QString("%1").arg(price);
    ui->label_PriceEcon->setText(PriceStr);

    PrimalPrice = this->PrimalPriceQuery(flightID,"0",orderstart,orderend);//公务舱的原价
    TotalDiscount = this->DiscountQuery(flightID,dep_date,"0");//公务舱的折扣
    ui->label_DiscountBusi->setText(TotalDiscount);
    price = PrimalPrice.toFloat()*TotalDiscount.toFloat();
    PriceStr = QString("%1").arg(price);
    ui->label_PriceBusi->setText(PriceStr);

    this->BalanceRefresh();

    ui->pushButton->setText(tr("Purchase"));
    ui->pushButton_Refresh->setText(tr("Refresh"));

    qDebug()<<dep_date<<endl;
    qDebug()<<dep_time<<endl;
    qDebug()<<dep_date+" "+dep_time<<endl;
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

//用于查询对应日期的航班的对应区间的座位存留量
QString Ticket_Purchase::TicketsLeftQuery(QString flightID, QString depDate, QString order_start, QString order_end, QString classType)
{
        QString sql_remaining_tickets = QString("CALL remaining_tickets_num('%1','%2',%3,%4,%5)"
                                                ).arg(flightID).arg(depDate).arg(order_start).arg(order_end).arg(classType);

        QSqlQuery * query_ticketsLeft = new QSqlQuery();
        query_ticketsLeft->exec(sql_remaining_tickets);
        QString TicketsLeft = "0";//如果查询无结果，默认为0，无余票！
        if(query_ticketsLeft->next()){
            TicketsLeft = query_ticketsLeft->value(0).toString();
        }
        return TicketsLeft;
}

QString Ticket_Purchase::getRandomString(int length)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());//为随机值设定一个seed

    //const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const char chrs[] = "0123456789";
    int chrs_size = sizeof(chrs);

    char* ch = new char[length + 1];
    memset(ch, 0, length + 1);
    int randomx = 0;
    for (int i = 0; i < length; ++i)
    {
        randomx= rand() % (chrs_size - 1);
        ch[i] = chrs[randomx];
    }

    QString ret(ch);
    delete[] ch;
    return ret;
}

void Ticket_Purchase::TicketsLeftRefresh() //更新一下界面的ticketLeft
{
    ui->label_remainEcon->setText(this->TicketsLeftQuery(this->flightID,this->depature_date,this->orderstart,this->orderend,"1"));//显示经济舱票价
    ui->label_reaminingBusi->setText(this->TicketsLeftQuery(this->flightID,this->depature_date,this->orderstart,this->orderend,"0"));//显示公务舱余票
}

void Ticket_Purchase::BalanceRefresh()
{
    float balance = acct->getMoney();
    QString money_string = QString("%1").arg(balance);
    ui->label_Balance->setText(tr("Balance:"));
    ui->label_BalanceMoney->setText(money_string);//设置账户余额
}

void Ticket_Purchase::Payment(flight_inquiry_citys_and_date *parent1,flight_inquiry_flightID *parent2,
                              QString UserID, QString balance, QString price, QString flightID,
                              QString dep_date, QString dep_time,
                              QString order_start, QString order_end, QString classType,QString companyID)
{
    //能进入则说明机票价一定小于等于用户账户余额
    float newBalance = balance.toFloat()-price.toFloat(); //计算支付票价后的账户余额
    QString ticketID = companyID+this->getRandomString(11); //生成合法的 ticket_id
    QString dep_datetime = dep_date+" "+dep_time;
    QString sql = QString("SELECT ticket_id FROM ticket WHERE ticket_id='%1'").arg(ticketID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    while (query->next()) {//只要有重复，就继续生成随机数，直至无重复
        ticketID = companyID+this->getRandomString(11);
        sql = QString("SELECT ticket_id FROM ticket WHERE ticket_id='%1'").arg(ticketID);
        query->clear();
        query->exec(sql);
    }
    //接下来写一个SQL语句，用事务表示，执行4个已经写好的存储过程，确保数据库中数据的一致性
    //4个存储过程分别位：balanceRefresh、 TicketsRecordInsertion、TicketsPurchaseRecordInsertion、TicketsLeftNumRefresh

    this->TicketsLeftRefresh();
    if(classType==1){
        if(ui->label_remainEcon->text().toInt()==0){
            QMessageBox::information(this,tr("Hint:"),tr("There are no tickets LEFT for economy class!"));
            return;
        }
    }else{
        if(ui->label_reaminingBusi->text().toInt()==0){
            QMessageBox::information(this,tr("Hint:"),tr("There are no tickets LEFT for business class!"));
            return;
        }
    }
    //执行一次事务，保持数据一致性
    sql = QString("BEGIN; "
                  "CALL balanceRefresh('%1',%2); "
                  "CALL TicketsRecordInsertion('%3','%4','%5','%6',%7,%8,%9); "
                  "CALL TicketsPurchaseRecordInsertion('%10','%11',%12); "
                  "CALL TicketsLeftNumRefresh('%13','%14',%15,%16,%17); "
                  "COMMIT;").arg(UserID).arg(newBalance).arg(ticketID).arg(UserID).arg(flightID)
            .arg(dep_datetime).arg(order_start).arg(order_end).arg(classType).arg(UserID).arg(ticketID)
            .arg(price).arg(flightID).arg(dep_date).arg(order_start).arg(order_end).arg(classType);
    query->clear();
    qDebug()<<sql<<endl;
    bool ok = query->exec(sql);
    if(ok){//说明订票操作成功，提醒用户查看其账户记录
        QMessageBox::information(this,tr("Hint:"),tr("Successful! "
                                                     "Please remember to check your orders in your account."));
        //并且更新账户里面的余额
        acct->setMoney(newBalance);
        if(parent2 == nullptr){ //购票成功后直接返回到用户的账户界面
            parent1->close();
        }else{
            parent2->close();
        }
        this->close();
        return;
    }else{//说明本次事务执行出现了异常，提示卖家再试一次
        QMessageBox::information(this,tr("Hint:"),tr("Failed. Please try again."));
        return;
    }
}



void Ticket_Purchase::on_pushButton_Refresh_clicked()
{
    this->BalanceRefresh();
    this->TicketsLeftRefresh();
}



//Btn: purchase
void Ticket_Purchase::on_pushButton_clicked()
{
    qDebug()<<"你刚刚点击了购买按钮！"<<endl;
    //检查此时能否购票，即查询一遍有无余票剩余，以及相关属性是否正常
    //若可以购票，则相应的有如下的结果：（如下结果按照事务来处理！）
    //用户自身的余额要扣除相应对等的数额（如果数额不够，则提示“余额不足，无法购票”）
    //ticket里面增加相应记录，ticket_purchase里面也要增加相应记录
    //余票数目减一，也即是相应航程覆盖的区间的剩余票数都减一
    //由于购票只算人头，那么值机的部分与此独立
    if(ui->radioButton_econ->isChecked()){//用户选择购买经济舱
        if(ui->label_PriceEcon->text()=="-1"){
            QMessageBox::information(this,tr("Hint:"),tr("This flight is NOT available now."));
            return;
        }
        this->TicketsLeftRefresh();
        if(ui->label_remainEcon->text()=="0"){
            QMessageBox::information(this,tr("Hint:"),tr("There are no tickets LEFT for economy class!"));
            return;
        }
        //具体的买票细节
        if(ui->label_BalanceMoney->text().toFloat()<ui->label_PriceEcon->text().toFloat()){
            //账户余额不足，提示充值
            QMessageBox::information(this,tr("Hint:"),tr("Your account balance is not enough. Please recharge your account."));
            return;
        }
        // 购票标准成立，则接下进行具体的后续改动
        qDebug()<<"正在进行购票处理，请稍等..."<<endl;

        QString moneyStr = QString("%1").arg(acct->getMoney());
        //完成支付
        this->Payment(this->ptr_CD,this->ptr_flightID,acct->getUserID(),moneyStr,ui->label_PriceEcon->text(),this->flightID,this->depature_date
                      ,this->departure_time,this->orderstart,this->orderend,"1",this->flightID.mid(0,2));//航班号的前两位为航空公司的代码
        return;

    }else{//用户选择公务舱
        if(ui->label_PriceBusi->text()=="-1"){
            QMessageBox::information(this,tr("Hint:"),tr("This flight is NOT available now."));
            return;
        }
        this->TicketsLeftRefresh();
        if(ui->label_reaminingBusi ->text()=="0"){
            QMessageBox::information(this,tr("Hint:"),tr("There are no tickets LEFT for business class!"));
            return;
         }
        //具体的买票细节
        if(ui->label_BalanceMoney->text().toFloat()<ui->label_PriceBusi->text().toFloat()){
            //账户余额不足，提示充值
            QMessageBox::information(this,tr("Hint:"),tr("Your account balance is not enough. Please recharge your account."));
            return;
        }
        // 购票标准成立，则接下进行具体的后续改动
        qDebug()<<"正在进行购票处理，请稍等..."<<endl;
        QString moneyStr = QString("%1").arg(acct->getMoney());
        //完成支付
        this->Payment(this->ptr_CD,this->ptr_flightID,acct->getUserID(),moneyStr,ui->label_PriceBusi->text(),this->flightID,this->depature_date
                      ,this->departure_time,this->orderstart,this->orderend,"0",this->flightID.mid(0,2));//航班号的前两位为航空公司的代码
        return;

   }
}
