#ifndef SEAT_SELECTION_H
#define SEAT_SELECTION_H

#include <QWidget>
#include <QTabWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

namespace Ui {
class seat_selection;
}

class seat_selection : public QWidget
{
    Q_OBJECT

public:
    explicit seat_selection(QWidget *parent = nullptr,QString flightID="",QString classType="1",QString dep_date=""
            ,QString UserID="",QString order_start="0",QString order_end="-1");
    ~seat_selection();

    QString flightTypeQuery(QString flightID);
    QString businessNoQuery(QString flightID);
    QString economyNoQuery(QString flightID);


private slots:
    void on_pushButton_Cancel_clicked();

private:
    Ui::seat_selection *ui;
    QString flightID;
    QString flightType;
    QString business_No;
    QString economy_No;
    QString classType;
    QString dep_date;// 出发时间
    QString UserID; //将用户与对应座位绑定
    QString order_start;
    QString order_end;
    //flightID、flightType、busiNo、econNo将决定整个座位区域
    //classType 表示用户可以选择的座位区域
    //flightID与后面四项确定哪些座位将被该用户占据

    void tableHeadersSet(QString flightType);
    void tableContentsSet(QString flightType,int busiNo,int econNo,QString classType);
};

//class MySeatTable:public QTableWidget{
//public:
//    QVariant data(,)
//};

//class CancelledOrderModel:public QSqlQueryModel{
//public:
//    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
//};

#endif // SEAT_SELECTION_H
