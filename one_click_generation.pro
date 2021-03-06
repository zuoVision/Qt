#模块
QT       += core gui svg network

#高于4版本添加widget
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#应用程序可双击执行，没有这行，只能在命令行./OneClick运行
QMAKE_LFLAGS += -no-pie

#icon
RC_ICONS += icon/superman.ico

#应用程序名
TARGET = OneClick

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
    BaseModule.cpp \
    ProcessorImpl.cpp \
    commandprocessthread.cpp \
    commoncommand.cpp \
    dialogbatterystats.cpp \
    document.cpp \
    fileoperation.cpp \
    listenerthread.cpp \
    main.cpp \
    mainwindow.cpp \
    mytableview.cpp \
    simpleperf.cpp \
    ssh.cpp \
    utils/parseXml/parseXml.cpp \
    xts.cpp

HEADERS += \
    BaseModule.h \
    ProcessorImpl.h \
    cmd.h \
    commandprocessthread.h \
    commoncommand.h \
    dialogbatterystats.h \
    document.h \
    fileoperation.h \
    listenerthread.h \
    mainwindow.h \
    mytableview.h \
    simpleperf.h \
    ssh.h \
    utils/general/general.h \
    utils/parseXml/parseXml.h \
    xts.h

FORMS += \
    dialogbatterystats.ui \
    document.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    config_file.qrc

#copy file/folder
CONFIG += file_copies

#addfile.files = $$PWD/filename
#addfile.path = $$OUT_PWD/

adddir.files = $$PWD/config
adddir.path = $$OUT_PWD/

COPIES += addfile
COPIES += adddir
