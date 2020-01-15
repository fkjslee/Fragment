QT       += core gui

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
    entity/fragmententity.cpp \
    ui/desktop.cpp \
    eventgraphicsscene.cpp \
    fragment.cpp \
    ui/fragmentarea.cpp \
    fragmentscontroller.cpp \
    ui/fragmentui.cpp \
    ui/hintwindow.cpp \
    main.cpp \
    ui/mainwindow.cpp \

HEADERS += \
    Tool.h \
    entity/fragmententity.h \
    ui/desktop.h \
    eventgraphicsscene.h \
    fragment.h \
    ui/fragmentarea.h \
    fragmentscontroller.h \
    ui/fragmentui.h \
    ui/hintwindow.h \
    ui/mainwindow.h \

FORMS += \
    ui/desktop.ui \
    ui/fragmentarea.ui \
    ui/hintwindow.ui \
    ui/mainwindow.ui


INCLUDEPATH += C:\Users\fkjslee\Downloads\opencv\build\include\opencv2 \
        C:\Users\fkjslee\Downloads\opencv\build\include

LIBS += C:\Users\fkjslee\Downloads\opencv\build\x64\vc14\lib\opencv_world411.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    rscs.qrc

TRANSLATIONS += translator_cn.ts
CODECFORTR=utf-8
