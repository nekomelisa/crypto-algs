QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        bi_to_ba.cpp \
        karatsuba.cpp \
        main.cpp \
        mod_inverse.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH +=/Users/nekomelisa/cppProjects/LIBS/qca/lib/qca-qt5.framework/Headers

LIBS += -F/Users/nekomelisa/cppProjects/LIBS/qca/lib -framework qca-qt5

HEADERS += \
    mod_inverse.h

LIBS+= -L/opt/homebrew/Cellar/openssl@3/3.4.0/lib -lssl -lcrypto
INCLUDEPATH += /opt/homebrew/Cellar/openssl@3/3.4.0/include
