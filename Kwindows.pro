#-------------------------------------------------
#
# Project created by QtCreator 2017-06-09T21:30:27
#
#-------------------------------------------------

QT       += core gui sql axcontainer multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Kwindows
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


RC_FILE = Kwindows.rc

RESOURCES += \
    BasicForm/kmessagebox.qrc

FORMS += \
    BasicForm/kmessagebox.ui \
    BasicForm/kr_listwidget.ui \
    BasicForm/kr_login.ui \
    BasicForm/kr_mainwindow.ui \
    Settingform/kr_about.ui \
    Settingform/kr_setting.ui \
    TaskForm/kr_addtask.ui \
    TaskForm/kr_trashstation.ui \
    ToolForm/kr_app.ui \
    ToolForm/kr_book.ui \
    ToolForm/kr_class.ui \
    ToolForm/kr_translate.ui \
    ToolForm/kr_welcome.ui

HEADERS += \
    BasicForm/kmessagebox.h \
    BasicForm/kr_listwidget.h \
    BasicForm/kr_login.h \
    BasicForm/kr_mainwindow.h \
    BasicForm/ktitlebar.h \
    BasicForm/kwindows.h \
    ScreenShot/CHotKeyEventFilter.hpp \
    ScreenShot/fullscreen.h \
    Settingform/kr_about.h \
    Settingform/kr_setting.h \
    TaskForm/kr_addtask.h \
    TaskForm/kr_trashstation.h \
    ToolForm/kr_app.h \
    ToolForm/kr_book.h \
    ToolForm/kr_class.h \
    ToolForm/kr_translate.h \
    ToolForm/kr_welcome.h

SOURCES += \
    BasicForm/kmessagebox.cpp \
    BasicForm/kr_listwidget.cpp \
    BasicForm/kr_login.cpp \
    BasicForm/kr_mainwindow.cpp \
    BasicForm/ktitlebar.cpp \
    BasicForm/kwindows.cpp \
    ScreenShot/CHotKeyEventFilter.cpp \
    ScreenShot/fullscreen.cpp \
    Settingform/kr_about.cpp \
    Settingform/kr_setting.cpp \
    TaskForm/kr_addtask.cpp \
    TaskForm/kr_trashstation.cpp \
    ToolForm/kr_app.cpp \
    ToolForm/kr_book.cpp \
    ToolForm/kr_class.cpp \
    ToolForm/kr_translate.cpp \
    ToolForm/kr_welcome.cpp \
    main.cpp
