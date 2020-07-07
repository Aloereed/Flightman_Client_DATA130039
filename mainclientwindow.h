#ifndef MAINCLIENTWINDOW_H
#define MAINCLIENTWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainClientWindow;
}

class MainClientWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainClientWindow(QWidget *parent = nullptr);
    ~MainClientWindow();

  private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_toolButton_clicked();

  private:
    Ui::MainClientWindow *ui;
};

#endif // MAINCLIENTWINDOW_H
