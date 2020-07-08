#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget {
    Q_OBJECT

  public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

  private slots:
    void on_comboBox_activated(int index);

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
