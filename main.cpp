#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QFile>
QSettings settings;
QByteArray readTextFile(const QString &file_path) {
  QFile input_file(file_path);
  QByteArray input_data;

  if (input_file.open(QIODevice::Text | QIODevice::Unbuffered | QIODevice::ReadOnly)) {
    input_data = input_file.readAll();
    input_file.close();
    return input_data;
  }
  else {
    return QByteArray();
  }
}


int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    QString style_sheet = readTextFile(settings.value("theme",":/qss/ElegantDark.qss").toString());
    a.setStyleSheet(style_sheet);
    MainWindow w;
    w.show();
    return a.exec();
}
