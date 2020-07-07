#include "settings.h"
#include "ui_settings.h"
#include "mainclientwindow.h"
#include <QTranslator>
#include <QSettings>
extern MainClientWindow *w;
extern QTranslator translator;
extern QSettings settings;
Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings) {
    ui->setupUi(this);
    ui->comboBox->setCurrentIndex(settings.value("Client/Langcase", 2).toInt());
    QApplication::setFont((settings.value("Client/UIFont",this->font()).value<QFont>()));
    ui->fontComboBox->setCurrentFont((settings.value("Client/UIFont", this->font()).value<QFont>()));
    //on_comboBox_activated(settings.value("Client/Langcase", 2).toInt());

}

Settings::~Settings() {
    delete ui;
}

void Settings::on_comboBox_activated(int index) {
    switch(index) {
    case 0: {
        qApp->removeTranslator(&translator);

        ui->retranslateUi(this);
        settings.setValue("Client/Langcase", 0);
        w->retranslateme();
        break;
    }

    case 1: {
        qApp->removeTranslator(&translator);
        QString langdir = ":/client_zh_CN.qm";
        translator.load(langdir);
        qApp->installTranslator(&translator);

        ui->retranslateUi(this);
        settings.setValue("Client/Langcase", 1);
        w->retranslateme();
        break;
    }

    default : {
        qApp->removeTranslator(&translator);
        QString langdir = ":/client_" + QLocale::system().name() + ".qm";
        translator.load(langdir);
        qApp->installTranslator(&translator);

        ui->retranslateUi(this);
        w->retranslateme();
        settings.setValue("Client/Langcase", 2);
    }
    }

}

void Settings::on_fontComboBox_currentFontChanged(const QFont &f) {

    settings.setValue("Client/UIFont", f);
    QApplication::setFont(f);

}

void Settings::on_pushButton_clicked() {
    settings.remove("Client/Langcase");
    settings.remove("Client/UIFont");

}
