#ifndef FLIGHT_INQUIRY_CITYS_AND_DATE_H
#define FLIGHT_INQUIRY_CITYS_AND_DATE_H

#include <QWidget>
#include <QString>
#include <QSqlQueryModel>

namespace Ui {
class flight_inquiry_citys_and_date;
}

class flight_inquiry_citys_and_date : public QWidget
{
    Q_OBJECT

public:
    explicit flight_inquiry_citys_and_date(QWidget *parent = nullptr,QString UserID = "",QString Password = ""
            ,QString Mode = "CD",QString Name="");
    ~flight_inquiry_citys_and_date();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_Flights_clicked(const QModelIndex &index);

private:
    Ui::flight_inquiry_citys_and_date *ui;
    QString UserID;
    QString Password;
    QString Mode;
    QString Name;
};

class MyQueryMode:public QSqlQueryModel{
    public:
        QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    };


#endif // FLIGHT_INQUIRY_H
