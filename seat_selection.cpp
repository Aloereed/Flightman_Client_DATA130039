#include "seat_selection.h"
#include "ui_seat_selection.h"
#include <QSqlQuery>

seat_selection::seat_selection(QWidget *parent,QString flightID,QString classType,QString dep_date
        ,QString UserID,QString order_start,QString order_end) :
    QWidget(parent),
    ui(new Ui::seat_selection)
{
    ui->setupUi(this);
    ui->label_hint->setText(tr("Please click on the cell to select your seat."));
    ui->label_colorhint1->setText(tr("white : available"));
    ui->label_colorhint2->setText(tr("gray : NOT available"));
    ui->pushButton_Cancel->setText(tr("Cancel"));

    //赋值
    this->UserID = UserID;
    this->flightID = flightID;
    this->classType = classType;
    this->dep_date = dep_date;
    this->order_start = order_start;
    this->order_end = order_end;
    this->flightType = flightTypeQuery(flightID); //0是单通道，1是双通道
    this->business_No = businessNoQuery(flightID);
    this->economy_No = economyNoQuery(flightID);

    int MaxBuisRows = this->business_No.toInt();
    int MaxEconRows = this->economy_No.toInt();

    ui->label_hintBuis->setText(tr("Business Class Row :"));
    ui->label_NoBuis->setText(" 1—"+QString("%1").arg(MaxBuisRows));

    ui->label_hintEcon->setText(tr("Economy Class Row :"));
    ui->label_NoEcon->setText(" "+QString("%1").arg(MaxBuisRows+1)+"—"+QString("%1").arg(MaxBuisRows+MaxEconRows));

    ui->label_hintSeat->setText(tr("Your seat :"));
    ui->label_seat->setText(classType=="0"?"Business":"Economy");

    ui->tableWidget_seats->setRowCount(this->business_No.toInt()+this->economy_No.toInt());
    int colum = this->flightType=="0"?7:11;
    ui->tableWidget_seats->setColumnCount(colum);

    this->tableHeadersSet(this->flightType);
    this->tableContentsSet(this->flightType,MaxBuisRows,MaxEconRows,this->classType);


//    QTableWidgetItem *columnHeaderItem0 = ui->tableWidget_seats->horizontalHeaderItem(0);
//    columnHeaderItem0->setFont(QFont("Helvetica")); //设置字体
//    columnHeaderItem0->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
//    columnHeaderItem0->setTextColor(QColor(200,111,30)); //设置文字颜色



}

seat_selection::~seat_selection()
{
    delete ui;
}

QString seat_selection::flightTypeQuery(QString flightID)
{
    QString flightType = "1";
    QString sql = QString("SELECT type FROM seat WHERE flight_id='%1'").arg(flightID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){
        flightType = query->value(0).toString();
    }
    return flightType;
}

QString seat_selection::businessNoQuery(QString flightID)
{
    QString businessNo = "10";
    QString sql = QString("SELECT `business_No.` FROM seat WHERE flight_id='%1'").arg(flightID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){
        businessNo = query->value(0).toString();
    }
    return businessNo;
}

QString seat_selection::economyNoQuery(QString flightID)
{
    QString economyNo = "30";
    QString sql = QString("SELECT `economy_No.` FROM seat WHERE flight_id='%1'").arg(flightID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->next()){
        economyNo = query->value(0).toString();
    }
    return economyNo;
}

void seat_selection::on_pushButton_Cancel_clicked()
{
    this->close();
}

void seat_selection::tableHeadersSet(QString flightType)
{
    QStringList headers;
    if(flightType=="0"){//表示为单通道
        headers << QStringLiteral("A") << QStringLiteral("B") << QStringLiteral("C")
                <<QStringLiteral("过道")<<QStringLiteral("J")<<QStringLiteral("K")
               <<QStringLiteral("L");
    }else{  //表示为双通道
        headers << QStringLiteral("A") << QStringLiteral("B") << QStringLiteral("C")<<QStringLiteral("过道")
                <<QStringLiteral("D")<<QStringLiteral("E")<<QStringLiteral("G")<<QStringLiteral("过道")
               <<QStringLiteral("H")<<QStringLiteral("J")<<QStringLiteral("K");
    }
    ui->tableWidget_seats->setHorizontalHeaderLabels(headers);
    return;
}

void seat_selection::tableContentsSet(QString flightType, int busiNo, int econNo, QString classType)
{
    int cols = flightType=="0"?7:11;
    int rows = busiNo + econNo;
    int classTypeInt = classType.toUInt();

    if(cols==7){ //说明本次航班是单通道的
            if(classTypeInt == 0){ //说明用户要选择单通道的公务舱
                for(int i=0;i<rows;i++){
                    for(int j=0;j<cols;j++){
                        if(j==3) continue;
                        if(i<busiNo && (j==1 || j==5)) continue;
                        ui->tableWidget_seats->setItem(i,j, new QTableWidgetItem("💺"));
                    }
            }
        }
    }else{ //本次航班是双通道的
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                if(j==3 || j==7) continue;
                if(i<busiNo && (j==1 || j==5 || j==9)) continue;
                ui->tableWidget_seats->setItem(i,j, new QTableWidgetItem("💺"));
            }

        }
    }
    //tableWidget->setItem(0,1,new QTableWidgetItem(QIcon(":/Image/IED.png"), "Jan's month"));

}
