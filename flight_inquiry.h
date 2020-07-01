#ifndef FLIGHT_INQUIRY_H
#define FLIGHT_INQUIRY_H

#include <QWidget>

namespace Ui {
class flight_inquiry;
}

class flight_inquiry : public QWidget
{
    Q_OBJECT

public:
    explicit flight_inquiry(QWidget *parent = nullptr);
    ~flight_inquiry();

private slots:
    void on_CancelpushButton_clicked();

    void on_BackpushButton_clicked();

    void on_QuerypushButton_clicked();

private:
    Ui::flight_inquiry *ui;
};

#endif // FLIGHT_INQUIRY_H
