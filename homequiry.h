#ifndef HOMEQUIRY_H
#define HOMEQUIRY_H

#include <QWidget>

namespace Ui {
class HomeQuiry;
}

class HomeQuiry : public QWidget {
    Q_OBJECT

  public:
    explicit HomeQuiry(QWidget *parent = nullptr);
    ~HomeQuiry();

  private slots:
    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    void on_dateEdit_dateChanged(const QDate &date);

    void on_dateEdit_2_dateChanged(const QDate &date);

    void on_dateEdit_3_dateChanged(const QDate &date);

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();

  private:
    Ui::HomeQuiry *ui;
};

#endif // HOMEQUIRY_H
