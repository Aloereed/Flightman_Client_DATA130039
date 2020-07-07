#ifndef TICKETANDSEAT_INFO_INTERFACE_H
#define TICKETANDSEAT_INFO_INTERFACE_H

#include <QWidget>

namespace Ui {
class ticketANDseat_info_interface;
}

class ticketANDseat_info_interface : public QWidget
{
    Q_OBJECT

public:
    explicit ticketANDseat_info_interface(QWidget *parent = nullptr,QString ticketID="",QString price="",QString flightID="",
                                          QString dep_airport="",QString arv_airport="",QString dep_datetime="",
                                          QString classType="",QString seatID="");

    QString airportNameQuery(QString airportID);

    ~ticketANDseat_info_interface();

private slots:
    void on_pushButton_cancel_clicked();

private:
    Ui::ticketANDseat_info_interface *ui;
};

#endif // TICKETANDSEAT_INFO_INTERFACE_H
