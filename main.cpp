#include "mainwindow.h"

#include <QtDebug>
#include <QApplication>
#include <fragment.h>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <desktop.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QApplication::setWindowIcon(QIcon(":/new/pre/resources/fragment.png"));
    QApplication::setStyle("fusion");
    w.showMaximized();
//    w.showFullScreen();
    w.show();
    return a.exec();
}
