#ifndef FLIGHT_INQUIRY_FLIGHTID_H
#define FLIGHT_INQUIRY_FLIGHTID_H

#include <QWidget>
#include <QString>

namespace Ui {
class flight_inquiry_flightID;
}

class flight_inquiry_flightID : public QWidget
{
    Q_OBJECT

public:
    explicit flight_inquiry_flightID(QWidget *parent = nullptr,QString UserID="",QString Password="");
    ~flight_inquiry_flightID();

private slots:
    void on_Cancel_pushButton_clicked();

    void on_Back_pushButton_clicked();

private:
    Ui::flight_inquiry_flightID *ui;
    QString UserID;
    QString Password;
};

#endif // FLIGHT_INQUIRY_FLIGHTID_H
