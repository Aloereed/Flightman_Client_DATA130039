QT       += core gui
QT       += sql
android{
    QT += androidextras
}
QT += core-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account_and_orders.cpp \
    chineseletterhelper.cpp \
    flight_inquiry.cpp \
    flight_inquiry_citys_and_date.cpp \
    flight_inquiry_flightid.cpp \
    homequiry.cpp \
    login.cpp \
    main.cpp \
    mainclientwindow.cpp \
    mainwindow.cpp \
    messagebox.cpp \
    recharge.cpp \
    registration.cpp \
    seat_selection.cpp \
    seat_selection_confirm.cpp \
    settings.cpp \
    textticker.cpp \
    ticket_purchase.cpp \
    ticket_purchase_confirm.cpp \
    ticket_refund_confirm.cpp \
    ticketandseat_info_interface.cpp

HEADERS += \
    account_and_orders.h \
    chineseletterhelper.h \
    flight_inquiry.h \
    flight_inquiry_citys_and_date.h \
    flight_inquiry_flightid.h \
    homequiry.h \
    login.h \
    mainclientwindow.h \
    mainwindow.h \
    messagebox.h \
    recharge.h \
    registration.h \
    seat_selection.h \
    seat_selection_confirm.h \
    settings.h \
    textticker.h \
    ticket_purchase.h \
    ticket_purchase_confirm.h \
    ticket_refund_confirm.h \
    ticketandseat_info_interface.h

RESOURCES += \
    loginbtn.qrc \
    png.qrc \
    style.qss

FORMS += \
    account_and_orders.ui \
    flight_inquiry.ui \
    flight_inquiry_citys_and_date.ui \
    flight_inquiry_flightid.ui \
    homequiry.ui \
    login.ui \
    mainclientwindow.ui \
    mainwindow.ui \
    messagebox.ui \
    recharge.ui \
    registration.ui \
    seat_selection.ui \
    seat_selection_confirm.ui \
    settings.ui \
    ticket_purchase.ui \
    ticket_purchase_confirm.ui \
    ticket_refund_confirm.ui \
    ticketandseat_info_interface.ui

TRANSLATIONS += \
    client_zh_CN.ts
RESOURCES += \
    client_zh_CN.qm \
    qss/Aqua.qss \
    qss/ElegantDark.qss \
    png/icon.png

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_EXTRA_LIBS +=    \ # modify the path
        $$PWD/libmariadb.so

DISTFILES += \
    android_sources/AndroidManifest.xml \
    android_sources/png/icon.png  \
    png/air.png \
    png/airreplace.png \
    png/background.jpg \
    png/logout.png \
    png/mine.png \
    png/msg.png \
    png/return.png \
    png/settings.png



ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android_sources
