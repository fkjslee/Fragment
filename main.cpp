#include "mainwindow.h"

#include <QtDebug>
#include <QApplication>
#include <fragment.h>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <desktop.h>
#include <set>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
