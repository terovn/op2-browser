######################################################################
# Automatically generated by qmake (2.01a) Mon Oct 6 19:47:24 2008
######################################################################

TEMPLATE = app
TARGET = 
CONFIG += uitools release
CONFIG -= app_bundle debug
unix:LIBS += -lX11
QT += network
DEPENDPATH += .
INCLUDEPATH += . \
        ../../kernel/
include($$PWD/../WebKit/WebKit.pri)

# Input
RESOURCES = html.qrc
HEADERS += webview.h webpage.h OpNetwork.h OpCookieJar.h OpNetworkReply.h \
           ../../kernel/Message.h OpNetworkAccessManager.h jswindow.h OpDownloader.h \
           FrameInit.h
SOURCES += main.cpp webview.cpp webpage.cpp OpNetwork.cpp OpCookieJar.cpp \
           OpNetworkReply.cpp ../../kernel/Message.cpp OpNetworkAccessManager.cpp \
           jswindow.cpp OpDownloader.cpp FrameInit.cpp


QMAKE_RPATHDIR = $$OUTPUT_DIR/lib $$QMAKE_RPATHDIR
