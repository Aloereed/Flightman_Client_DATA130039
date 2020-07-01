#ifndef REGISTRATION_H
#define REGISTRATION_H
#include <QAbstractButton>
#include <QWidget>

namespace Ui {
class registration;
}

class registration : public QWidget
{
    Q_OBJECT

public:
    explicit registration(QWidget *parent = nullptr);
    ~registration();

private slots:
    void on_pushButton_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::registration *ui;
};

typedef struct UserInfo
{
    QString ID;
    QString name;
    QString passwordmd5;
}my_user;

#endif // REGISTRATION_H
