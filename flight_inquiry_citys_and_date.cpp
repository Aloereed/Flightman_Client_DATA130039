#include "flight_inquiry_citys_and_date.h"
#include "ui_flight_inquiry_citys_and_date.h"
#include "flight_inquiry.h"
#include "mainclientwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
flight_inquiry_citys_and_date::flight_inquiry_citys_and_date(QWidget *parent,QString UserID,QString Password, QString Mode) : QWidget(parent), ui(new Ui::flight_inquiry_citys_and_date)
{
    ui->setupUi(this);

    this->UserID = UserID;
    this->Password = Password;
    this->Mode = Mode;

    if (UserID == ""){
        ui->UserStatus->setText("User Information: Not logged in");
    }
    else{
        QString sql = QString("select name from [user] where ID = '%1'").arg(UserID);
        QSqlQuery * query = new QSqlQuery();
        query->exec(sql);
        if (query->next()){
            QString UserName = query->value(0).toString();
            ui->UserStatus->setText("User Information: "+UserName);
        }
        else{//即便有用户ID输入，但是数据库内无相关用户ID的信息，所以视为用户登录失败，显示未登录；
            ui->UserStatus->setText("User Information: Not logged in");
        }
    }

    //只查询起降城市，不需要起飞日期
    if(Mode == 'C'){
        ui->label_3->hide();
        ui->DepartureDate->hide();
    }

    ui->DepatureCity->setEchoMode(QLineEdit::Normal);
    ui->DepatureCity->setPlaceholderText("Please Enter Your Depature City.");

    ui->ArrivalCity->setEchoMode(QLineEdit::Normal);
    ui->ArrivalCity->setPlaceholderText("Please Enter Your Arrival City.");

    ui->DepartureDate->setEchoMode(QLineEdit::Normal);
    ui->DepartureDate->setPlaceholderText("Please Enter Your Depature Date. E.g. YY-MM-DD.");

    //layout()->setSizeConstraint(QLayout::SetFixedSize);

}

flight_inquiry_citys_and_date::~flight_inquiry_citys_and_date()
{
    delete ui;
}

//Btn: Back
void flight_inquiry_citys_and_date::on_pushButton_2_clicked()
{
    flight_inquiry * flt_inq = new flight_inquiry();
    flt_inq->show();
    this->close();
}

//Btn:Cancel
void flight_inquiry_citys_and_date::on_pushButton_3_clicked()
{
    qDebug()<<"You decide to quit."<<endl;
    QApplication::quit();
}

//Btn:Confirm- for searching
void flight_inquiry_citys_and_date::on_pushButton_clicked()
{
    qDebug()<<Mode<<endl;
    QString dep_city = ui->DepatureCity->text();
    QString arv_city = ui->ArrivalCity->text();
    QString dep_date = ui->DepartureDate->text();

    //允许只按照起降城市查询；
    if (dep_city == ""||arv_city == ""){
            QMessageBox::critical(this,"incomplete input.","critical");
            return;
    }

    if (this->Mode == "CD" && dep_date==""){
        QMessageBox::critical(this,"incomplete input.","critical");
        return;
    }
    //模仿例子，与整段代码无关
    //QSqlQueryModel *model = new QSqlQueryModel;
     //   model->setQuery("select * from student");
     //   model->setHeaderData(0, Qt::Horizontal, tr("id"));
     //   model->setHeaderData(1, Qt::Horizontal, tr("name"));
     //   QTableView *view = new QTableView;
     //   view->setModel(model);
     //   view->show();

    //用于创造航班查询的视图
    QString sql1;
    if (this->Mode == "CD")
    {
        sql1 = QString("ALTER VIEW flight_inquiry_citys_and_date AS "
                            "SELECT e.*,f.airport_id,f.arrival_time,f.departure_time,f.city FROM "
                             "( "
                             "SELECT a.flight_id,schedule,departure_datetime FROM "
                             "( "
                             "(SELECT flight_id,schedule FROM flight) AS a  "
                             "INNER JOIN "
                             "(SELECT CONVERT(flight_arrangment.departure_datetime,date) AS departure_datetime,flight_id FROM flight_arrangment WHERE flight_arrangment.departure_datetime>now() AND CONVERT(flight_arrangment.departure_datetime,date)='%1') AS b "
                             "ON a.flight_id=b.flight_id "
                             ") "
                             ") AS e "
                             "INNER JOIN "
                             "( "
                             "SELECT c.airport_id,c.arrival_time,c.departure_time,c.flight_id,d.city FROM "
                             "( "
                             "(SELECT * FROM airline) AS c "
                             "INNER JOIN "
                             "(SELECT airport.airport_id,city FROM airport) AS d "
                             "ON c.airport_id=d.airport_id  "
                             ") "
                             ")AS f "
                             "ON e.flight_id=f.flight_id").arg(dep_date);
    }
    else{
        sql1 = QString("ALTER VIEW flight_inquiry_citys_and_date AS "
                             "SELECT e.*,f.airport_id,f.arrival_time,f.departure_time,f.city FROM "
                             "( "
                             "SELECT a.flight_id,schedule,departure_datetime FROM "
                             "( "
                             "(SELECT flight_id,schedule FROM flight) AS a  "
                             "INNER JOIN "  //取消了对航班出发日期的限制
                             "(SELECT CONVERT(flight_arrangment.departure_datetime,date) AS departure_datetime,flight_id FROM flight_arrangment ) AS b "
                             "ON a.flight_id=b.flight_id "
                             ") "
                             ") AS e "
                             "INNER JOIN "
                             "( "
                             "SELECT c.airport_id,c.arrival_time,c.departure_time,c.flight_id,d.city FROM "
                             "( "
                             "(SELECT * FROM airline) AS c "
                             "INNER JOIN "
                             "(SELECT airport.airport_id,city FROM airport) AS d "
                             "ON c.airport_id=d.airport_id  "
                             ") "
                             ")AS f "
                             "ON e.flight_id=f.flight_id"); //只做结合航班信息的操作
    }

    qDebug()<<sql1<<endl;
    QSqlQuery * view = new QSqlQuery();
    view->exec(sql1);
    //用于航班查询的视图已创建好

    QString sql2 = QString("SELECT table_begin.*,table_end.arrival_city,table_end.arrival_airport,table_end.arrival_time FROM "
                           "( "
                           "( "
                           "SELECT departure_datetime,schedule,flight_id,city as departure_city,airport_id as departure_airport,departure_time FROM flight_inquiry_citys_and_date WHERE flight_inquiry_citys_and_date.city='%1' "
                           ") AS table_begin "
                           "INNER JOIN "
                           "( "
                           "SELECT flight_id,city as arrival_city,airport_id as arrival_airport,arrival_time FROM flight_inquiry_citys_and_date WHERE flight_inquiry_citys_and_date.city='%2' "
                           ") AS table_end "
                           "ON table_begin.flight_id=table_end.flight_id "
                           ") "
                           "WHERE departure_time IS NOT NULL AND arrival_time IS NOT NULL AND departure_time<arrival_time").arg(dep_city).arg(arv_city);
    //需要对查询条件再加个飞机的起飞时间在现在之前，这一部分可以放在sql查询之前判断，如果查询日期<当今日期，则无法查询 (已确保查询的显示的航班信息在当前时间之后√)
    //另一方面如果查询日期>=当今日期，则分为(当今日期>查询日期)和(当今日期=查询日期 && 查询时间>当前时间);

    qDebug()<<sql2<<endl;

    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(sql2);
    model->setHeaderData(0,Qt::Horizontal,QString::fromUtf8(tr("Departure Date").toUtf8()));
    model->setHeaderData(1,Qt::Horizontal,QString::fromUtf8(tr("Schedule").toUtf8()));
    model->setHeaderData(2,Qt::Horizontal,QString::fromUtf8(tr("flight_id").toUtf8()));
    model->setHeaderData(3,Qt::Horizontal,QString::fromUtf8(tr("Departure City").toUtf8()));
    model->setHeaderData(4,Qt::Horizontal,QString::fromUtf8(tr("Departure Airport").toUtf8()));
    model->setHeaderData(5,Qt::Horizontal,QString::fromUtf8(tr("Departure Time").toUtf8()));
    model->setHeaderData(6,Qt::Horizontal,QString::fromUtf8(tr("Arrival City").toUtf8()));
    model->setHeaderData(7,Qt::Horizontal,QString::fromUtf8(tr("Arrival Airport").toUtf8()));
    model->setHeaderData(8,Qt::Horizontal,QString::fromUtf8(tr("Arrival Time").toUtf8()));

    //还需要实现每一行的"购票"的按钮；



    ui->Flights->setModel(model);
    ui->Flights->show();
    //建议，把航班表的起飞和终到全部拿到航线里面；

}
