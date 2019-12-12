#include "mainwindow.h"

#include <QtDebug>
#include <QApplication>
#include <iostream>
#include <coloritem.h>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <desktop.h>
#include <set>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    std::cout << "main function call" << std::endl;
    w.show();

    return a.exec();
}
