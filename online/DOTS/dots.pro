QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = dots
CONFIG -= console
#CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    DotsSimplifier.cpp \
    Helper.cpp \
    DotsException.cpp \
    PersistenceBatchSimplifier.cpp \
    mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    DouglasPeuckerBatchSimplifier.cpp \
    SquishBatchSimplifier.cpp \
    AlgorithmComparison.cpp

HEADERS += \
    DotsSimplifier.h \
    Helper.h \
    DotsException.h \
    PersistenceBatchSimplifier.h \
    mainwindow.h \
    qcustomplot/qcustomplot.h \
    psimpl/psimpl.h \
    DouglasPeuckerBatchSimplifier.h \
    SquishBatchSimplifier.h \
    AlgorithmComparison.h

FORMS += \
    mainwindow.ui

