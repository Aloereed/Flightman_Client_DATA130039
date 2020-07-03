#ifndef FLIGHT_INQUIRY_FLIGHTID_H
#define FLIGHT_INQUIRY_FLIGHTID_H

#include <QWidget>
#include <QString>
#include <QSqlQueryModel>

namespace Ui {
class flight_inquiry_flightID;
}

class flight_inquiry_flightID : public QWidget
{
    Q_OBJECT

public:
    explicit flight_inquiry_flightID(QWidget *parent = nullptr,QString UserID="",QString Password="",QString Name = "");
    ~flight_inquiry_flightID();

private slots:
    void on_Cancel_pushButton_clicked();

    void on_Back_pushButton_clicked();

    void on_Confirm_pushButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_login_pushButton_clicked();

private:
    Ui::flight_inquiry_flightID *ui;
    QString UserID;
    QString Password;
    QString Name;
};

class MyQueryMode_fid:public QSqlQueryModel{
    public:
        QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    };


#endif // FLIGHT_INQUIRY_FLIGHTID_H
