QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/opencv4

LIBS += -L/usr/lib/x86_64-linux-gnu/ -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
LIBS += -L$$PWD/ -lzmq

SOURCES += \
    data.cpp \
    main.cpp \
    mainwindow.cpp \
    zmq_subscriber.cpp

HEADERS += \
    data.hpp \
    mainwindow.h \
    zmq.hpp \
    zmq_subscriber.hpp

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    aiView_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
