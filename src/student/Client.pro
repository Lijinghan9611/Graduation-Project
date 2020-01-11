#-------------------------------------------------
#
# Project created by QtCreator 2016-11-30T20:06:10
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
RC_FILE += app.rc
TARGET = ZQUCLIENT
TEMPLATE = app

UI_DIR=./UI
SOURCES += main.cpp\
        MainWindow.cpp \
    Config.cpp \
    Sign.cpp \
    Question.cpp \
    CmdThread.cpp \
    CmdSend.cpp \
    BrocastLabel.cpp \
    BrocastThread.cpp \
    MsgLabel.cpp \
    login.cpp \
    stoast.cpp \
    fileinfodialog.cpp

HEADERS  += MainWindow.h \
    Config.h \
    Global.h \
    Sign.h \
    Question.h \
    CmdThread.h \
    CmdSend.h \
    BrocastLabel.h \
    BrocastThread.h \
    MsgLabel.h \
    login.h \
    stoast.h \
    fileinfodialog.h

FORMS    += mainwindow.ui \
    Config.ui \
    login.ui \
    fileinfodialog.ui

RESOURCES += \
    images.qrc
