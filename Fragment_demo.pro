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
    CommonHeader.cpp \
    commands.cpp \
    fragmentsscene.cpp \
    hintscene.cpp \
    network.cpp \
    storescene.cpp \
    ui/refreshthread.cpp \
    ui/storearea.cpp \
    ui/fragmentarea.cpp \
    fragmentscontroller.cpp \
    ui/fragmentui.cpp \
    ui/hintwindow.cpp \
    main.cpp \
    ui/mainwindow.cpp

HEADERS += \
    CommonHeader.h \
    Tool.h \
    commands.h \
    fragmentsscene.h \
    hintscene.h \
    network.h \
    storescene.h \
    ui/refreshthread.h \
    ui/storearea.h \
    ui/fragmentarea.h \
    fragmentscontroller.h \
    ui/fragmentui.h \
    ui/hintwindow.h \
    ui/mainwindow.h

FORMS += \
    ui/storearea.ui \
    ui/fragmentarea.ui \
    ui/hintwindow.ui \
    ui/mainwindow.ui


#INCLUDEPATH += /usr/local/include \
#                /usr/local/include/opencv4 \
#                /usr/local/lib/eigen \
#                /usr/local/lib/include \
#                /usr/local/include/opencv4\opencv2 \
#                /usr/include/python2.7
##                -I/usr/include/python2.7 -I/usr/include/x86_64-linux-gnu/python2.7  -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fdebug-prefix-map=/build/python2.7-5Z483E/python2.7-2.7.17=. -fstack-protector-strong -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes

#LIBS += /usr/local/lib/libopencv_highgui.so \
#        /usr/local/lib/libopencv_core.so    \
#        /usr/local/lib/libopencv_imgproc.so \
#        /usr/local/lib/libopencv_imgcodecs.so \
#        -L/usr/lib/python2.7/config-x86_64-linux-gnu -L/usr/lib -lpython2.7 -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions


INCLUDEPATH += C:\opencv3\build\include \
                C:\opencv3\build\include\opencv2 \
                -I C:\Users\fkjslee\anaconda3\envs\python27\include \
                C:\Users\fkjslee\anaconda3\envs\python27\Lib\site-packages\numpy\core\include

LIBS += C:\opencv3\build\x64\vc15\lib\opencv_world430.lib \
    -LC:\Users\fkjslee\anaconda3\envs\python27\libs -lpython27 \
    -LC:\Users\fkjslee\anaconda3\envs\python27\Lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    rscs.qrc

TRANSLATIONS += translator_cn.ts
CODECFORTR=utf-8

DISTFILES += \
    FusionImage.py
