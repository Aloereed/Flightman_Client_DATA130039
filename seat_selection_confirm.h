#ifndef SEAT_SELECTION_CONFIRM_H
#define SEAT_SELECTION_CONFIRM_H

#include <QWidget>
#include "seat_selection.h"

namespace Ui {
class seat_selection_confirm;
}

class seat_selection_confirm : public QWidget
{
    Q_OBJECT

public:
    explicit seat_selection_confirm(QWidget *parent = nullptr,seat_selection *parent_true=nullptr,QString seatID="");
    ~seat_selection_confirm();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_confirm_clicked();

private:
    Ui::seat_selection_confirm *ui;
    seat_selection *parent;
    QString seatID;
};

#endif // SEAT_SELECTION_CONFIRM_H
