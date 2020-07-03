#ifndef FLIGHT_INQUIRY_H
#define FLIGHT_INQUIRY_H

#include <QWidget>
#include <QString>

namespace Ui {
class flight_inquiry;
}

class flight_inquiry : public QWidget
{
    Q_OBJECT

public:
    explicit flight_inquiry(QWidget *parent = nullptr,QString UserID = "",QString Pwd = "",QString Name="");
    ~flight_inquiry();

    void greeting(QString ID,QString Password);

private slots:
    void on_CancelpushButton_clicked();

    void on_BackpushButton_clicked();

    void on_QuerypushButton_clicked();

private:
    Ui::flight_inquiry *ui;

    QString UserID;
    QString Pwd;
    QString Name;
};

#endif // FLIGHT_INQUIRY_H
