#-------------------------------------------------
#
# Project created by QtCreator 2015-06-20T12:11:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GraphCombEx
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    graphs.cpp \
    random_generator.cpp \
    common.cpp \
    statistics.cpp \
    dialogba.cpp \
    dialogudg.cpp \
    algorithm.cpp \
    problem.cpp \
    algorithm_brelaz.cpp \
    algorithm_greedyindset.cpp \
    algorithm_greedyclique.cpp \
    algorithm_iggcc.cpp \
    problem_ccp.cpp \
    dialogcompletetree.cpp \
    algorithm_greedydom.cpp \
    algorithm_igcol.cpp \
    dialoggendom.cpp \
    dialogws.cpp \
    largevisform.cpp \
    evovisform.cpp \
    algorithm_lscycle.cpp \
    dialogoptionsalgorithms.cpp \
    dialoggrid.cpp \
    dialogoptionsvisualization.cpp

HEADERS  += mainwindow.h \
    graphs.h \
    random_generator.h \
    common.h \
    statistics.h \
    dialogba.h \
    dialogudg.h \
    algorithm.h \
    problem.h \
    algorithm_brelaz.h \
    algorithm_greedyindset.h \
    algorithm_greedyclique.h \
    algorithm_iggcc.h \
    problem_ccp.h \
    dialogcompletetree.h \
    algorithm_greedydom.h \
    algorithm_igcol.h \
    dialoggendom.h \
    dialogws.h \
    largevisform.h \
    evovisform.h \
    algorithm_lscycle.h \
    dialogoptionsalgorithms.h \
    dialoggrid.h \
    dialogoptionsvisualization.h

FORMS    += mainwindow.ui \
    dialogba.ui \
    dialogudg.ui \
    dialogcompletetree.ui \
    dialoggendom.ui \
    dialogws.ui \
    largevisform.ui \
    evovisform.ui \
    dialogoptionsalgorithms.ui \
    dialoggrid.ui \
    dialogoptionsvisualization.ui
