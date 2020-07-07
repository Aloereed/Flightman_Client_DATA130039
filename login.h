#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QPushButton>
namespace Ui {
class login;
}

class login : public QWidget {
    Q_OBJECT

  public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    void hideforhome();
    bool isAuto();
  public slots:
    void on_pushButton_clicked();

  private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();



    void on_checkBox_2_clicked();

    void on_checkBox_clicked();

  private:
    Ui::login *ui;
};

#endif // LOGIN_H
