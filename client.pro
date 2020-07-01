QT       += core gui
QT       += sql

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
    flight_inquiry.cpp \
    flight_inquiry_citys_and_date.cpp \
    flight_inquiry_flightid.cpp \
    login.cpp \
    main.cpp \
    mainclientwindow.cpp \
    mainwindow.cpp \
    recharge.cpp \
    registration.cpp

HEADERS += \
    account_and_orders.h \
    flight_inquiry.h \
    flight_inquiry_citys_and_date.h \
    flight_inquiry_flightid.h \
    login.h \
    mainclientwindow.h \
    mainwindow.h \
    recharge.h \
    registration.h

FORMS += \
    account_and_orders.ui \
    flight_inquiry.ui \
    flight_inquiry_citys_and_date.ui \
    flight_inquiry_flightid.ui \
    login.ui \
    mainclientwindow.ui \
    mainwindow.ui \
    recharge.ui \
    registration.ui

TRANSLATIONS += \
    client_zh_CN.ts
RESOURCES += \
    client_zh_CN.qm

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_EXTRA_LIBS +=    \ # modify the path
        $$PWD/libmariadb.so
