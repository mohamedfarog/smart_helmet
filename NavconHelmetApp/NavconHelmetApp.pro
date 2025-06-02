INCLUDEPATH += .

QT += quick bluetooth
CONFIG += c++14
# Input
SOURCES += main.cpp \
    device.cpp \
    deviceinfo.cpp \
    serviceinfo.cpp \
    characteristicinfo.cpp \
    DfuManager.cpp
OTHER_FILES += assets/*.qml

HEADERS += \
    device.h \
    deviceinfo.h \
    serviceinfo.h \
    characteristicinfo.h \
    DfuManager.h
RESOURCES += \
    resources.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/bluetooth/lowenergyscanner
INSTALLS += target

LIBS += -lssl -lcrypto -lqca-qt5

#DISTFILES += \
#    assets/farog.qml

DISTFILES += \
    assets/Dfu.qml


