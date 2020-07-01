#ifndef FLIGHT_INQUIRY_CITYS_AND_DATE_H
#define FLIGHT_INQUIRY_CITYS_AND_DATE_H

#include <QWidget>
#include <QString>

namespace Ui {
class flight_inquiry_citys_and_date;
}

class flight_inquiry_citys_and_date : public QWidget
{
    Q_OBJECT

public:
    explicit flight_inquiry_citys_and_date(QWidget *parent = nullptr,QString UserID = "",QString Password = "",QString Mode = "CD");
    ~flight_inquiry_citys_and_date();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

private:
    Ui::flight_inquiry_citys_and_date *ui;
    QString UserID;
    QString Password;
    QString Mode;
};

#endif // FLIGHT_INQUIRY_H
