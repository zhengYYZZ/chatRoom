#-------------------------------------------------
#
# Project created by QtCreator 2019-01-31T22:40:37
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatRoom
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        login.cpp \
    basewindow.cpp \
    mytitlebar.cpp \
    tcpclient.cpp \
    tcpserver.cpp \
    charroom.cpp

HEADERS += \
        login.h \
    basewindow.h \
    mytitlebar.h \
    tcpclient.h \
    tcpserver.h \
    charroom.h

FORMS += \
        login.ui \
    tcpclient.ui \
    tcpserver.ui \
    charroom.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
