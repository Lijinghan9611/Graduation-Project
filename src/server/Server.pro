#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T13:16:06
#
#-------------------------------------------------

QT       += core gui network sql
QT +=xlsx
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_FILE += app.rc

TARGET = ZQUSERVER
TEMPLATE = app

CONFIG(debug, debug|release) {
    LIBS += -lQt5Ftpd
} else {
    LIBS += -lQt5Ftp
}

UI_DIR=./UI
SOURCES += main.cpp\
        MainWindow.cpp \
    UiConfigDialog.cpp \
    NetConfigDialog.cpp \
    Tool_Bar.cpp \
    MornitorView.cpp \
    LabelView.cpp \
    Global.cpp \
    Server.cpp \
    ServerThread.cpp \
    ThreadSend.cpp \
    LabelStyle.cpp \
    ZoomLabel.cpp \
    CLassManage.cpp \
    ComboboxItem.cpp \
    MyMessageBox.cpp \
    CommonTitleBar.cpp \
    CoolTitleBar.cpp \
    CmdWidget.cpp \
    CmdLineEdit.cpp \
    ScreenBrocast.cpp \
    ScreenBrocast2.cpp \
    LogIn.cpp \
    SignIn.cpp \
    SendMg.cpp \
    MyButton.cpp \
    WorkLog.cpp \
    AdminOption.cpp \
    ClientAtribute.cpp \
    TranportFileWidget.cpp \
    ComparePortrait.cpp \
    mysqltablemodel.cpp \
    ftpmanager.cpp \
    ExcelManager.cpp \
    stoast.cpp

HEADERS  += MainWindow.h \
    UiConfigDialog.h \
    NetConfigDialog.h \
    Tool_Bar.h \
    MornitorView.h \
    LabelView.h \
    Global.h \
    Server.h \
    ServerThread.h \
    ThreadSend.h \
    LabelStyle.h \
    ZoomLabel.h \
    CLassManage.h \
    ComboboxItem.h \
    MyMessageBox.h \
    CommonTitleBar.h \
    CoolTitleBar.h \
    CmdWidget.h \
    CmdLineEdit.h \
    ScreenBrocast.h \
    ScreenBrocast2.h \
    LogIn.h \
    SignIn.h \
    SendMg.h \
    MyButton.h \
    WorkLog.h \
    AdminOption.h \
    ClientAtribute.h \
    TranportFileWidget.h \
    ComparePortrait.h \
    mysqltablemodel.h \
    ftpmanager.h \
    ExcelManager.h \
    stoast.h

FORMS    += MainWindow.ui \
    UiConfigDialog.ui \
    NetConfigDialog.ui \
    CLassManage.ui \
    MyMessageBox.ui \
    CommonTitleBar.ui \
    login.ui \
    SignIn.ui \
    WorkLog.ui \
    AdminOption.ui \
    ClientAtribute.ui \
    TranportFileWidget.ui \
    ComparePortrait.ui

RESOURCES += \
    image.qrc \
    qssresource.qrc \
    files.qrc

