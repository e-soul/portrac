QT += widgets
TEMPLATE = app
TARGET = portrac
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lfetch -lbz2

# Input
HEADERS += PortChecker.hh Trac.hh Preferences.hh PreferencesDialog.hh Downloader.hh PortracException.hh Environment.hh Logger.hh Decompressor.hh
SOURCES += PortChecker.cc Trac.cc Preferences.cc PreferencesDialog.cc Downloader.cc PortracException.cc Environment.cc Logger.cc Decompressor.cc main.cc
