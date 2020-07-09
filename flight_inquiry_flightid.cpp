#include "flight_inquiry_flightid.h"
#include "ui_flight_inquiry_flightid.h"
#include "flight_inquiry.h"
#include "account_and_orders.h"
#include "login.h"
#include "ticket_purchase.h"
#include <QDebug>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>
#include<QScroller>
extern account_and_orders * acct; //只有当用户进入到账户界面后，也即UserID!=""时，可以有效使用

flight_inquiry_flightID::flight_inquiry_flightID(QWidget *parent,QString UserID,QString Password,QString Name) :
    QWidget(parent),
    ui(new Ui::flight_inquiry_flightID)
{
    ui->setupUi(this);
    QFile styleFile(":/style.qss");//使用qss文件
    styleFile.open(QIODevice::ReadOnly);//只读
    QString setStyleSheet(styleFile.readAll());//读取所有的
    ui->tableView->setStyleSheet(setStyleSheet);//设置样式
    this->UserID = UserID;
    this->Password = Password;
    this->Name = Name;
    QScroller::grabGesture(ui->tableView, QScroller::TouchGesture);
    ui->hint_label->setText(tr("Flight ID"));
    ui->flighID_lineEdit->setPlaceholderText(tr("Please enter the flight ID you want to query."));
    ui->flighID_lineEdit->setEchoMode(QLineEdit::Normal);
    ui->Back_pushButton->setText(tr("Back"));
    ui->flightInfo_label->setText(tr("Flight Information:"));
    ui->Confirm_pushButton->setText(tr("Confirm"));
    ui->Cancel_pushButton->setText(tr("Cancel"));
    ui->login_pushButton->setText(tr("Log In"));

    if (UserID==""){
        ui->UserStatus->setText(tr("User Information: Not logged in"));
    }
    else{
        ui->UserStatus->setText(tr("User Information: ")+Name);
        ui->login_pushButton->hide();
    }
}

flight_inquiry_flightID::~flight_inquiry_flightID()
{
    delete ui;
}

void flight_inquiry_flightID::on_Cancel_pushButton_clicked()
{
    qDebug()<<"You decied to quit."<<endl;
    this->close();
}

void flight_inquiry_flightID::on_Back_pushButton_clicked()
{
    flight_inquiry * flt_inq = new flight_inquiry(nullptr,this->UserID,this->Password,this->Name);
    flt_inq->show();
    this->close();
}



void flight_inquiry_flightID::on_Confirm_pushButton_clicked()
{
    QString flightID = ui->flighID_lineEdit->text();
    if (flightID == ""){
        QMessageBox::critical(this,tr("Critical"),tr("Incomplete input."));
        return;
    }
    QString sql = QString("CALL flight_query_ID('%1')").arg(flightID+"%");

    qDebug()<<sql<<endl;

    MyQueryMode_fid *model = new MyQueryMode_fid();
    model->setQuery(sql);
    model->setHeaderData(0,Qt::Horizontal,QString::fromUtf8(tr("Departure Date").toUtf8()));
    model->setHeaderData(1,Qt::Horizontal,QString::fromUtf8(tr("Flight ID").toUtf8()));
    model->setHeaderData(2,Qt::Horizontal,QString::fromUtf8(tr("Schedule").toUtf8()));
    model->setHeaderData(3,Qt::Horizontal,QString::fromUtf8(tr("Departure Airport ID").toUtf8()));
    model->setHeaderData(4,Qt::Horizontal,QString::fromUtf8(tr("Departure Airport").toUtf8()));
    model->setHeaderData(5,Qt::Horizontal,QString::fromUtf8(tr("Departure City").toUtf8()));
    model->setHeaderData(6,Qt::Horizontal,QString::fromUtf8(tr("Departure Time").toUtf8()));
    model->setHeaderData(7,Qt::Horizontal,QString::fromUtf8(tr("Departure Positon").toUtf8()));
    model->setHeaderData(8,Qt::Horizontal,QString::fromUtf8(tr("Arrival Airport ID").toUtf8()));
    model->setHeaderData(9,Qt::Horizontal,QString::fromUtf8(tr("Arrival Airport").toUtf8()));
    model->setHeaderData(10,Qt::Horizontal,QString::fromUtf8(tr("Arrival City").toUtf8()));
    model->setHeaderData(11,Qt::Horizontal,QString::fromUtf8(tr("Arrival Time").toUtf8()));
    model->setHeaderData(12,Qt::Horizontal,QString::fromUtf8(tr("Arrival Positon").toUtf8()));
    model->insertColumn(13); // 插入购买列
    model->setHeaderData(13,Qt::Horizontal,QString::fromUtf8(tr("Booking").toUtf8()));
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
}


QVariant MyQueryMode_fid::data(const QModelIndex &item, int role) const{
    QVariant value = QSqlQueryModel::data(item,role);
    if (role == Qt::BackgroundColorRole){
        if(item.column()==13)
            return QVariant::fromValue(QColor(225,225,225));
    }
    if (role == Qt::DisplayRole){
        if(item.column()==7 || item.column()==12){
            if(QSqlQueryModel::data(item).toInt()==0)
                return QVariant::fromValue(QString(tr("Of Daparture")));
            else if(QSqlQueryModel::data(item).toInt()==-1)
                return QVariant::fromValue(QString(tr("Of Arrival")));
            else{
                QString order = QSqlQueryModel::data(item).toString();
                QString info =tr("No.") + order + tr(" Transfer");
                return QVariant::fromValue(info);
            }
        }
        if(item.column()==13)
            return QVariant::fromValue(tr("Booking"));
        }
        return value;
}




void flight_inquiry_flightID::on_tableView_clicked(const QModelIndex &index)
{
    if (index.isValid()&&index.column()==13){ //点击订票键，打开相关的UI界面，显示价格并进行购票操作
        //如果没有登陆，则提示登陆账户后再买票，并结束该界面，进入到登陆界面
        //如果登陆了，则打开购票信息框
        qDebug()<<"你刚刚点击了订票按钮"<<endl;
        if(this->UserID==""){
            //说明是以游客形式预定的，提示其先进行账户登陆
            QMessageBox::information(this,tr("Hint:"),tr("Before booking, You need to log in first."));
            return;
        }
        //如果是非游客形式的预定，则先根据点击的内容获取该行的航班信息
        int row = index.row(); // 返回点击单元格的所在行数，从而根据行来提取相关航班的信息。
        QAbstractItemModel* model = ui->tableView->model();
        QString dep_date = model->data(model->index(row,0)).toString();
        QString fligh_id = model->data(model->index(row,1)).toString();
        QString schedule = model->data(model->index(row,2)).toString();
        QString dep_airportName = model->data(model->index(row,4)).toString();
        QString dep_city = model->data(model->index(row,5)).toString();
        QString dep_time = model->data(model->index(row,6)).toString();
        QString arv_airportName = model->data(model->index(row,9)).toString();
        QString arv_city = model->data(model->index(row,10)).toString();
        QString arv_time = model->data(model->index(row,11)).toString();
        QString order_start = model->data(model->index(row,7)).toString();
        QString order_end = model->data(model->index(row,12)).toString();

        if(order_start==tr("Of Daparture")) order_start="0";
        else if(order_start==tr("Of Arrival")) order_start="-1";
        else order_start=order_start.mid(1,1);

        if(order_end==tr("Of Daparture")) order_end="0";
        else if(order_end==tr("Of Arrival")) order_end="-1";
        else order_end=order_end.mid(1,1);

        //检查该用户是否已经购买过同一趟航班。如果是，则提示用户不能重复购买，并取消预定操作
        QString sql_check_doublebooking = QString("SELECT COUNT(*) FROM ticket WHERE flight_id='%1' "
                "AND ID='%2' AND CAST(departure_datetime AS date)='%3' AND ticket_id NOT IN "
                "(SELECT ticket_id FROM ticket_refund ) ")
                .arg(fligh_id).arg(acct->getUserID()).arg(dep_date);
//        "AND ID='%2' AND ( CAST(departure_datetime AS date)='%3' "
//        "OR  DATEDIFF(CAST(departure_datetime AS date),CAST('%4' AS date))=1 )")

        qDebug()<<sql_check_doublebooking<<endl;
        QSqlQuery *query_check_doublebooking = new QSqlQuery();
        query_check_doublebooking->exec(sql_check_doublebooking);
        query_check_doublebooking->next();//查询成功，则表明此用户此前曾购买过同一趟的飞机，则此时提示已购买同一趟飞机，并禁止购买
        if(query_check_doublebooking->value(0).toInt()){
            QMessageBox::information(this,tr("Hint:"),tr("You have already booked this flight. Please choose another flight."));
            return;
        }


        //根据所截取的信息来查询该趟航班是否有余票（在确认购买时仍需要查询是否有余票，以保持数据的一致性）
        QString sql_remaining_tickets_business = QString("CALL remaining_tickets_num("
                                                "'%1','%2',%3,%4,0)").arg(fligh_id).arg(dep_date).arg(order_start).arg(order_end);
        QString sql_remaining_tickets_economy = QString("CALL remaining_tickets_num("
                                                "'%1','%2',%3,%4,1)").arg(fligh_id).arg(dep_date).arg(order_start).arg(order_end);
        qDebug()<<sql_remaining_tickets_business<<endl;
        qDebug()<<sql_remaining_tickets_economy<<endl;

        int business_remaining_num=0;
        QSqlQuery *query_remaining_tickets_business = new QSqlQuery();
        query_remaining_tickets_business->exec(sql_remaining_tickets_business);
        if(query_remaining_tickets_business->next()){
            business_remaining_num = query_remaining_tickets_business->value(0).toInt();
        }
        int economy_remaining_num=0;
        QSqlQuery *query_remaining_tickets_econuomy = new QSqlQuery();
        query_remaining_tickets_econuomy->exec(sql_remaining_tickets_economy);
        if(query_remaining_tickets_econuomy->next()){
            economy_remaining_num = query_remaining_tickets_econuomy->value(0).toInt();
        }

        qDebug()<<"公务舱剩余座位数:"<<business_remaining_num<<endl;
        qDebug()<<"经济舱剩余座位数:"<<economy_remaining_num<<endl;

        if (business_remaining_num==0 && economy_remaining_num==0){
            QMessageBox::information(this,tr("Hint:"),tr("There are no tickets left on this flight. Please choose another flight."));
            return;
        }
        qDebug()<<"仍有余票，允许购买"<<endl;
        //否则，还剩余有机票可以购买，则进入到购票界面。

        Ticket_Purchase *purchase_interface = new Ticket_Purchase(nullptr,nullptr,this,dep_date,fligh_id,schedule,dep_airportName,dep_city,dep_time,
                                                                  arv_airportName,arv_city,arv_time,order_start,order_end);
        purchase_interface->show();

    }
}

void flight_inquiry_flightID::on_login_pushButton_clicked()
{
    login *lgin = new login();
    lgin->show();
    QApplication::processEvents();
    this->close();
}
