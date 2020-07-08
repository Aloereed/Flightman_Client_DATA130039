#include "account_and_orders.h"
#include "ui_account_and_orders.h"
#include "recharge.h"
#include "flight_inquiry.h"
#include "ticket_refund_confirm.h"
#include "seat_selection.h"
#include "login.h"
#include "ticketandseat_info_interface.h"
#include "messagebox.h"
#include <QDebug>
#include <QWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QFile>
#include <QAbstractItemModel>
#include<QScroller>
#include<QStackedWidget>
#include<QSettings>
#include<QTimer>
extern login *lgin;
extern QStackedWidget* mainstack;
//构造对象时，必须要给参数：ID 和 Pwd
//此处传入的Pwd是经过MD5转换后的密码，与数据库中所存储的密码对应
extern account_and_orders * acct;
extern QSettings settings;

account_and_orders::account_and_orders(QWidget *parent,QString ID,QString Pwd) : QWidget(parent), ui(new Ui::account_and_orders) {
    ui->setupUi(this);
    QFile styleFile(":/style.qss");//使用qss文件
    styleFile.open(QIODevice::ReadOnly);//只读
    QString setStyleSheet(styleFile.readAll());//读取所有的
    ui->coming_tableView->setStyleSheet(setStyleSheet);//设置样式
    QScroller::grabGesture(ui->coming_tableView, QScroller::TouchGesture);
    QScroller::grabGesture(ui->finished_tableView, QScroller::TouchGesture);
    QScroller::grabGesture(ui->cancelled_tableView, QScroller::TouchGesture);
    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    //根据登录界面给定的有效用户信息初始化账户界面
    this->UserID=ID;
    this->Password=Pwd;

    QString sql_user = QString("SELECT * FROM user WHERE ID = '%1' AND password = '%2'").arg(ID).arg(Pwd);
    QSqlQuery * query_user = new QSqlQuery();
    query_user->exec(sql_user);
    QString money_string = QString("");
    float money = 0;
    int membership = 0;
    QString name = QString("");
    if(query_user->next()) {
        money_string = query_user->value(3).toString();
        money = query_user->value(3).toInt();
        membership = query_user->value(2).toInt();
        name = query_user->value(1).toString();
    }

    this->Name = name;
    this->Money = money;
    this->Membership = membership;
    this->status = 0;
    this->hash_seatsInfoOfUser = this->HashSeatsOfUser(this->UserID);

    QString greeting = QString("Hello, %1").arg(this->Name);
    ui->User_label->setText(greeting);
    ui->account_label->setText(tr("Account Balance: ") + money_string + tr("￥"));

    if(membership == 1) {
        ui->memInfo_label->setText(tr("Membership: Yes"));
        ui->Buymem_pushButton->setDisabled(true);
        //成为会员之后不需要再购买会员
    } else {
        ui->memInfo_label->setText(tr("Membership: No"));
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
    //ui->MessageBox_pushButton->setText(tr("Messages"));
    ui->Hint_label->setText(tr("click 'Messages' to check NEW information."));
    ui->logoutButton->setMaximumWidth(ui->logoutButton->height());
    ui->logoutButton->setIconSize(ui->logoutButton->rect().size());
    ui->MessageBox_pushButton->setMaximumWidth(ui->MessageBox_pushButton->height());
    ui->MessageBox_pushButton->setIconSize(ui->MessageBox_pushButton->rect().size());
    this->on_Refresh_pushButton_clicked();
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkNewMsg()));
    timer.start(1000);
    qDebug()<<timer.isActive();
}

account_and_orders::~account_and_orders() {
    delete ui;
    acct=NULL;
}
void account_and_orders::checkNewMsg(){
    QSqlQuery ck(QString("SELECT time,text FROM announcement WHERE userID = '%1' OR userID='000000000000000000'").arg(this->UserID));
    if(ck.size()!=settings.value("User/MsgCount",0)){
        ui->MessageBox_pushButton->setIcon(QIcon(":/png/msg_new.png"));
    }
}

void account_and_orders::setMoney(float Money) {
    this->Money = Money;
}

float account_and_orders::getMoney() {
    return this->Money;
}

float account_and_orders::ticketActualRefundQuery(float actualPay,QString dep_datetime) {
    float actualRefund = actualPay;
    int TimeDistance = 0;
    QString sql = QString("SELECT UNIX_TIMESTAMP(CAST('%1' AS DATETIME)) - UNIX_TIMESTAMP(NOW())").arg(dep_datetime);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    query->first();
    TimeDistance = query->value(0).toInt();
    if(TimeDistance>=86400) { //距离起飞还有24小时以及更久
        actualRefund *= 0.90;
    } else if(TimeDistance>=7200) { //距离起飞还有2-24小时
        actualRefund *= 0.80;
    } else {
        actualRefund *= 0.65; //0.5-2小时
    }
    return actualRefund;
}

QString account_and_orders::getUserID() {
    return this->UserID;
}

QString account_and_orders::seatIDQuery(QString flightID, QString dep_date,QString passengerID) {
    QString seatID = ""; //默认无查询结果的话，座位号为空
    QString sql= QString("SELECT seat_id FROM seat_arrangement WHERE flight_id='%1' "
                         "AND departure_date='%2' AND passengerID='%3'")
                 .arg(flightID).arg(dep_date).arg(passengerID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()) { //说明是查询到了相关用户的座位信息，可以查询该用户在本次航程上的座位号
        seatID = query->value(0).toString();
    }
    //否则说明没有查询到乘客的座位信息，返回空字符串
    return seatID;
}

QString account_and_orders::ticketOrderStartQuery(QString ticketID) {
    QString depID = "0";
    QString sql = QString("SELECT departure_id FROM ticket WHERE ticket_id='%1'").arg(ticketID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()) {
        depID = query->value(0).toString();
    }
    return depID;
}

QString account_and_orders::ticketOrderEndQuery(QString ticketID) {
    QString arvID = "-1";
    QString sql = QString("SELECT arrival_id FROM ticket WHERE ticket_id='%1'").arg(ticketID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()) {
        arvID = query->value(0).toString();
    }
    return arvID;
}

QString account_and_orders::ticketActualPayQuery(QString ticketID) {
    QString actualPay = "0";
    QString sql = QString("SELECT actual_payment FROM ticket_purchase WHERE ticket_id='%1'").arg(ticketID);
    qDebug()<<sql<<endl;
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()) {
        actualPay = query->value(0).toString();
        qDebug()<<actualPay<<endl;
    }
    return actualPay;

}

QHash<QString, QString> account_and_orders::HashSeatsOfUser(QString passengerID)
{
    QHash<QString, QString> hash_flightInfoToseatid;
    QString sql = QString("SELECT flight_id,departure_date,seat_id FROM seat_arrangement"
                          " WHERE passengerID='%1'").arg(passengerID);
    QSqlQuery *query = new QSqlQuery();
    if(query->exec(sql)){//成功执行
        while (query->next()) {
            qDebug()<<query->value(0).toString()<<" || "<<query->value(1).toString()<<" || "<<query->value(2).toString()<<endl;
            hash_flightInfoToseatid.insert(query->value(0).toString()+" "+query->value(1).toString(),query->value(2).toString());
        }
    }
    hash_flightInfoToseatid.insert("0","-1"); //默认存量
    return  hash_flightInfoToseatid;
//    QHash<QString,QString> hash_seatidTopassengerid;
//    hash_seatidTopassengerid.insert("0","0");//默认存量
//    QString sql = QString("SELECT seat_id,passengerID FROM seat_arrangement WHERE flight_ID='%1' AND `order`=%2 AND "
//                          "departure_date='%3'")
//            .arg(flightID).arg(order_start).arg(dep_date);
//    QSqlQuery *query = new QSqlQuery();
//    if(query->exec(sql)){ //成功执行
//        while(query->next()){
//            qDebug()<<query->value(0).toString()<<"||"<<query->value(1).toString()<<endl;
//            hash_seatidTopassengerid.insert(query->value(0).toString(),query->value(1).toString());
//        }
//    }
    //    return hash_seatidTopassengerid;
}

QHash<QString, QString> account_and_orders::getHashSeatsOfUser()
{
    return this->hash_seatsInfoOfUser;
}

bool account_and_orders::IsCheckedIn(QString flightID, QString dep_date, QString passengerID)
{
    QString sql = QString("SELECT COUNT(*) FROM seat_arrangement WHERE flight_id='%1' AND "
                          "departure_date='%2' AND passengerID='%3' ").arg(flightID).arg(dep_date).arg(passengerID);
    qDebug()<<sql<<endl;
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    query->next();
    if(query->value(0).toInt()){//说明该乘客在对应的航班上有座位的记录，表示其已经值机了.
        return true;
    }
    return false;
}

void account_and_orders::BalanceRefresh() {
    QString money_string = QString("%1").arg(this->Money);
    ui->account_label->setText(tr("Account Balance: ") + money_string + tr("￥"));
}

int account_and_orders::getMembership() {
    return this->Membership;
}

int account_and_orders::getStatus() {
    return this->status;
}

void account_and_orders::setStatus(int status) {
    this->status = status;
}

void account_and_orders::on_Buymem_pushButton_clicked() {
    if (this->Money < 100) {
        QMessageBox::information(this,tr("Hint"),tr("Sorry, your balance is not enough."));
        return;
    }

    float money = this->Money - 100;

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
    if(ok) {
        this->Money -= 100;  //扣除会员费
        this->BalanceRefresh();
        ui->memInfo_label->setText(tr("Membership: Yes"));
        ui->Buymem_pushButton->setDisabled(true);

        QMessageBox::information(this,tr("Hint:"),tr("Your payment has been done."));
    } else {
        QMessageBox::information(this,tr("Hint:"),tr("Something wrong with your payment."));
    }


}

void account_and_orders::on_Recharge_pushButton_clicked() {
    recharge * rec = new recharge();
    rec->show();
}

void account_and_orders::on_Refresh_pushButton_clicked() {
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
    model_coming->insertColumn(7);//插入座位列
    model_coming->setHeaderData(7,Qt::Horizontal,QString::fromUtf8(tr("Seat").toUtf8()));
    model_coming->insertColumn(8); // 插入退票列
    model_coming->setHeaderData(8,Qt::Horizontal,QString::fromUtf8(tr("Refund").toUtf8()));
    model_coming->insertColumn(9); // 插入值机列
    model_coming->setHeaderData(9,Qt::Horizontal,QString::fromUtf8(tr("Check In").toUtf8()));
    model_coming->insertColumn(10);// 插入改签列
    model_coming->setHeaderData(10,Qt::Horizontal,QString::fromUtf8(tr("Rebooking").toUtf8()));
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


void account_and_orders::on_coming_tableView_clicked(const QModelIndex &index) {

    if (index.isValid()&&index.column()==8) { //点击退票键，检查退票标准是否成立
        // 如果已值机或者未值机但退票时间Now()距离起飞时间不到3小时，则不能退票；
        // 如果还未值机且时间还未到起飞前3小时，可以退票
        // 根据起飞时间判断返回票价
        qDebug()<<"你刚刚点击了退票按钮"<<endl; //若退票标准成立，则执行退票操作
        int row = index.row(); //获取点击的行数
        QAbstractItemModel *model = ui->coming_tableView->model();
        QString ticketID =  model->data(model->index(row,0)).toString();
        QString flightID = model->data(model->index(row,1)).toString();
        QString dep_datetime = model->data(model->index(row,2)).toString();
        dep_datetime = dep_datetime.mid(0,10)+" "+dep_datetime.mid(11,8);
        QString seatID = "";
        QString sql_pre = QString("SELECT seat_id FROM ticketVSseatid_view WHERE ticket_id='%1'").arg(ticketID);
        QSqlQuery *query_pre = new QSqlQuery();
        query_pre->exec(sql_pre);
        if(query_pre->next()){
            seatID = query_pre->value(0).toString();
        }
        this->on_Refresh_pushButton_clicked();
        if(seatID!="") { //如果完成了值机，则不允许退票
            QMessageBox::information(this,tr("Hint:"),tr("You have checked in. So you cannot refund your ticket."));
            return;
        }
        int TimeDistance = 0;
        QString sql = QString("SELECT UNIX_TIMESTAMP(CAST('%1' AS DATETIME)) - UNIX_TIMESTAMP(NOW())").arg(dep_datetime);
        qDebug()<<sql<<endl;
        QSqlQuery *query = new QSqlQuery();
        query->exec(sql);
        query->first();
        TimeDistance = query->value(0).toInt();
        if(TimeDistance <= 1800) { //退票时间距离起飞还有0.5小时
            QMessageBox::information(this,tr("Hint:"),tr("The flight is going to take off soon.So you cannot refund your ticket."));
            return;
        }
        //通过检验标准，可以进行退票操作
        //还是执行一个事务：
        //首先，需要将ticket中的相关票的信息插入到退票表中
        //然后，相应票对应的舱位剩余座位数+1
        //最后根据退票时间和起飞时间之间的差距，返回相应票价百分比的数额到用户
//        QString ticketID = model->data(model->index(row,0)).toString();
        QString order_start = this->ticketOrderStartQuery(ticketID);
        QString order_end = this->ticketOrderEndQuery(ticketID);
        QString classType = model->data(model->index(row,5)).toString();
        classType = classType=="Economy"?"1":"0";
        float actualPay = this->ticketActualPayQuery(ticketID).toFloat();
        float refundMoney = this->ticketActualRefundQuery(actualPay,dep_datetime);
        float newBalance = this->Money + refundMoney;
        acct->setMoney(newBalance);

        ticket_refund_confirm *refund_interface = new ticket_refund_confirm(nullptr,this->UserID,newBalance,ticketID
                ,refundMoney,flightID,dep_datetime
                ,order_start,order_end,classType);
        refund_interface->show();
    } else if (index.isValid()&&index.column()==9) { //点击值机，进行值机操作
        //
        qDebug()<<"你刚刚点击了值机按钮"<<endl; //选择座位等()
        int row = index.row(); //获取点击的行数
        QAbstractItemModel *model = ui->coming_tableView->model();
//        QString seatID =  model->data(model->index(row,7)).toString();
        QString dep_datetime = model->data(model->index(row,2)).toString();
        QString flightID = model->data(model->index(row,1)).toString();
        dep_datetime = dep_datetime.mid(0,10)+" "+dep_datetime.mid(11,8);
        QString classType = model->data(model->index(row,5)).toString();
        classType = classType=="Business"?"0":"1";
        QString ticketID = model->data(model->index(row,0)).toString();
        QString order_start = this->ticketOrderStartQuery(ticketID);
        QString order_end = this->ticketOrderEndQuery(ticketID);

        QString seatID = "";
        QString sql_pre = QString("SELECT seat_id FROM ticketVSseatid_view WHERE ticket_id='%1'").arg(ticketID);
        QSqlQuery *query_pre = new QSqlQuery();
        query_pre->exec(sql_pre);
        if(query_pre->next()){
            seatID = query_pre->value(0).toString();
        }

        this->on_Refresh_pushButton_clicked();
        if(seatID != "") { //如果完成了值机，则不允许退票
            QMessageBox::information(this,tr("Hint:"),tr("You have checked in. So you cannot Check In AGAIN."));
            return;
        }
        int TimeDistance = 0;
        QString sql = QString("SELECT UNIX_TIMESTAMP(CAST('%1' AS DATETIME)) - UNIX_TIMESTAMP(NOW())").arg(dep_datetime);
        qDebug()<<sql<<endl;
        QSqlQuery *query = new QSqlQuery();
        query->exec(sql);
        query->first();
        TimeDistance = query->value(0).toInt();
        if(TimeDistance <= 900) { //起飞时间距离起飞还有15分钟
            QMessageBox::information(this,tr("Hint:"),tr("The flight is going to take off soon. Check In service have been closed."));
            return;
        }
        //满足值机条件，进入值机界面
        qDebug()<<"满足条件，您即将进入值机页面"<<endl;
        seat_selection *checkIn_interface = new seat_selection(nullptr,model->data(model->index(row,1)).toString(),classType
                ,dep_datetime.mid(0,10),acct->getUserID(),order_start,order_end);
        checkIn_interface->show();
    } else if (index.isValid()&&index.column()==10) { //点击值机，进行值机操作
        //
        qDebug()<<"你刚刚点击了改签按钮"<<endl; //选择座位等()
    }else if(index.isValid()&&index.column()==7){ //点击查看个人的座位信息
        qDebug()<<"你刚刚点击了座位信息按钮"<<endl; //选择座位等()
        int row = index.row(); //获取点击的行数
        QAbstractItemModel *model = ui->coming_tableView->model();
        QString ticketID = model->data(model->index(row,0)).toString();
        QString dep_datetime = model->data(model->index(row,2)).toString();
        QString flightID = model->data(model->index(row,1)).toString();
        dep_datetime = dep_datetime.mid(0,10)+" "+dep_datetime.mid(11,8);

        QString dep_airport = model->data(model->index(row,3)).toString();
        QString arv_airport = model->data(model->index(row,4)).toString();
        QString classType = model->data(model->index(row,5)).toString();
        QString price = model->data(model->index(row,6)).toString();

        QString seatID = "";
        QString sql_pre = QString("SELECT seat_id FROM ticketVSseatid_view WHERE ticket_id='%1'").arg(ticketID);
        QSqlQuery *query_pre = new QSqlQuery();
        query_pre->exec(sql_pre);
        if(query_pre->next()){
            seatID = query_pre->value(0).toString();
        }
        ticketANDseat_info_interface *ticket_Interface = new ticketANDseat_info_interface(nullptr,ticketID,price,flightID,dep_airport,arv_airport
                                                                                          ,dep_datetime,classType,seatID);
        ticket_Interface->show();
    }

}

QVariant FinishedOrderModel::data(const QModelIndex &item, int role) const {
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::DisplayRole) {
        if(item.column()==5) {
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("Business")));
            else
                return QVariant::fromValue(QString(tr("Economy")));
        }
    }
    return value;
}


QVariant ComingOrderModel::data(const QModelIndex &item, int role) const {
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::BackgroundColorRole) {
        if(item.column()==8)
            return QVariant::fromValue(QColor(225,225,225));
        else if(item.column()==9)
            return QVariant::fromValue(QColor(225,225,225));//颜色特殊表示，起到类似按钮的作用
        else if(item.column()==10)
            return QVariant::fromValue(QColor(225,225,225));
        else if(item.column()==7)
            return QVariant::fromValue(QColor(225,225,225));
    }
    if (role == Qt::DisplayRole) {
        if(item.column()==5) {
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("Business")));
            else
                return QVariant::fromValue(QString(tr("Economy")));
        }
        if(item.column()==7) { //座位列显示的相关操作
//            int row = item.row();
//            QString ticket_id = this->data(this->index(row,0)).toString();     //ticketid
//            QString flight_id = this->data(this->index(row,1)).toString();
//            QString dep_date = this->data(this->index(row,2)).toString().mid(0,10);
//            QString key = flight_id+" "+dep_date;
//            QString seatID = acct->getHashSeatsOfUser().find(acct->getUserID()).value();
//            qDebug()<<"seat ID : "<<seatID<<endl;
//            if(seatID=="-1"){//说明该订单还未完成值机
//                return QVariant::fromValue(tr(""));
//            }else{//说明该订单已经完成值机，显示对应的座位号
//                return QVariant::fromValue(tr("1"));
//            }

//            QString order_start = acct->ticketOrderStartQuery(ticket_id);
//            QString order_end = acct->ticketOrderEndQuery(ticket_id);
            //不能直接用acct->getUserID()来获得userID，这样会形成死锁，因为这时acct还没有构建完毕
//            QString passengerID = "";
//            QString sql = QString("SELECT ID FROM ticket WHERE ticket_id='%1'").arg(ticket_id);
//            QSqlQuery *query = new QSqlQuery();
//            query->exec(sql);
//            if(query->next()){//说明该票的信息有效，记录UserID
//                passengerID = query->value(0).toString();
//            }
//            QString seatID = acct->seatIDQuery(flight_id,dep_date,passengerID);
//            return QVariant::fromValue(seatID);
            return QVariant::fromValue(tr("Seat Information"));
        }
        if(item.column()==8)
            return QVariant::fromValue(tr("Refund"));
        else if(item.column()==9)
            return QVariant::fromValue(tr("Check In"));
        else if(item.column()==10)
            return QVariant::fromValue(tr("Rebooking"));
    }
    return value;
}

QVariant CancelledOrderModel::data(const QModelIndex &item, int role) const {
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::DisplayRole) {
        if(item.column()==5) {
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("Business")));
            else
                return QVariant::fromValue(QString(tr("Economy")));
        }
    }
    return value;
}



void account_and_orders::on_buyticket_pushButton_clicked() {
    flight_inquiry * flight = new flight_inquiry(nullptr,this->UserID,this->Password,this->Name);
    flight->show();
    //ui->buyticket_pushButton->setDisabled(true); //不允许成功打开查询页面后重复查询.
}

void account_and_orders::on_logoutButton_clicked() {
    mainstack->setCurrentWidget(lgin);
    acct=NULL;
    delete this;
}

void account_and_orders::on_MessageBox_pushButton_clicked()
{
    messagebox *message_interface = new messagebox();
    message_interface->show();
    QSqlQuery ck(QString("SELECT time,text FROM announcement WHERE userID = '%1' OR userID='000000000000000000'").arg(acct->getUserID()));
    ck.next();
    settings.setValue("User/MsgCount",ck.size());
    ui->MessageBox_pushButton->setIcon(QIcon(":/png/msg.png"));
    ui->MessageBox_pushButton->setIconSize(ui->MessageBox_pushButton->rect().size());

}
