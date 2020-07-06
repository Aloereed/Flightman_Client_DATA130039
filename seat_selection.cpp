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

QString seat_selection::ComputeSeatID(int row, int col, QString flightType)
{
    QString rowID = QString("%1").arg(row+1);
    QString colID = "";
    if(flightType=="0"){ //单通道
        if(col==0){
            colID = "A";
        }else if(col==1){
            colID = "B";
        }
        else if(col==2){
            colID = "C";
        }else if(col==4){
            colID = "J";
        }else if(col==5){
            colID = "K";
        }else if(col==6){
            colID = "L";
        }
    }else{ //双通道
        if(col==0){
            colID = "A";
        }else if(col==1){
            colID = "B";
        }
        else if(col==2){
            colID = "C";
        }else if(col==4){
            colID = "D";
        }else if(col==5){
            colID = "E";
        }else if(col==6){
            colID = "G";
        }else if(col==8){
            colID = "H";
        }else if(col==9){
            colID = "J";
        }else if(col==10){
            colID = "K";
        }
    }
    return rowID+colID;
}

bool seat_selection::IsSeatInUsage(QString flightID, QString start_order, QString dep_date, QString seatID)
{
    QString sql = QString("SELECT passengerID FROM seat_arrangement WHERE flight_ID='%1' "
                          "AND `order`=%2 AND departure_date='%3' AND seat_id='%4'")
            .arg(flightID).arg(start_order).arg(dep_date).arg(seatID);
    QSqlQuery *query = new QSqlQuery();
    query->exec(sql);
    if(query->first()){//查询成功
        if(query->value(0).toString()==""?1:0){//查询到的passengerID为空，说明无人使用
            return false;
        }
        return true;
    }
    //查询失败，默认被占用
    return true;
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
    QString seatID = "";

    if(cols==7){ //说明本次航班是单通道的
            if(classTypeInt == 0){ //说明用户要选择单通道的公务舱
                for(int i=0;i<rows;i++){
                    for(int j=0;j<cols;j++){
                        if(j==3) continue; //位于过道
                        if(i<busiNo && (j==1 || j==5)) continue; //位于公务舱两个位置的间隔
                        seatID = this->ComputeSeatID(i,j,flightType);
                        ui->tableWidget_seats->setItem(i,j, new QTableWidgetItem("💺 "+seatID));
                        if(i>=business_No){ //|| (this->IsSeatInUsage(this->flightID,this->order_start,this->dep_date,seatID))){
                            QTableWidgetItem *item = new QTableWidgetItem();
                            item->setBackground(QColor(225,225,225));
                            ui->tableWidget_seats->setItem(i,j,item);
                        }
                    }
                }
            }else{//说明用户要选择单通道的经济舱
                for(int i=0;i<rows;i++){
                    for(int j=0;j<cols;j++){
                        if(j==3) continue; //位于过道
                        if(i<busiNo && (j==1 || j==5)) continue; //位于公务舱两个位置的间隔
                        seatID = this->ComputeSeatID(i,j,flightType);
                        ui->tableWidget_seats->setItem(i,j, new QTableWidgetItem("💺 "+seatID));
                        if(i<business_No){ //|| (this->IsSeatInUsage(this->flightID,this->order_start,this->dep_date,seatID))){
                            QTableWidgetItem *item = new QTableWidgetItem();
                            item->setBackground(QColor(225,225,225));
                            ui->tableWidget_seats->setItem(i,j,item);
                        }
                    }
                }
            }
    }else{ //本次航班是双通道的
        if(classTypeInt == 0){//说明用户要选择双通道的公务舱
            for(int i=0;i<rows;i++){
                for(int j=0;j<cols;j++){
                    if(j==3 || j==7) continue;
                    if(i<busiNo && (j==1 || j==5 || j==9)) continue;
                    seatID = this->ComputeSeatID(i,j,flightType);
                    ui->tableWidget_seats->setItem(i,j, new QTableWidgetItem("💺 "+seatID));
                    if(i>=business_No){ // || (this->IsSeatInUsage(this->flightID,this->order_start,this->dep_date,seatID))){
                        QTableWidgetItem *item = new QTableWidgetItem();
                        item->setBackgroundColor(QColor(225,225,225));
                        ui->tableWidget_seats->setItem(i,j,item);
                    }
                }
            }
        }else{
            for(int i=0;i<rows;i++){//说明用户要选择双通道的经济舱
                for(int j=0;j<cols;j++){
                    if(j==3 || j==7) continue;
                    if(i<busiNo && (j==1 || j==5 || j==9)) continue;
                    seatID = this->ComputeSeatID(i,j,flightType);
                    ui->tableWidget_seats->setItem(i,j, new QTableWidgetItem("💺 "+seatID));
                    if(i>=business_No  ){//|| (this->IsSeatInUsage(this->flightID,this->order_start,this->dep_date,seatID))){
                        QTableWidgetItem *item = new QTableWidgetItem();
                        item->setBackgroundColor(QColor(200,111,100));
                        ui->tableWidget_seats->setItem(i,j,item);
                    }
                }
            }
        }
    }
    //tableWidget->setItem(0,1,new QTableWidgetItem(QIcon(":/Image/IED.png"), "Jan's month"));
}
