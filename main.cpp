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


//    QGraphicsScene scene(-200, -200, 400, 400);
//    for (int i = 0; i < 10; ++i) {
//        ColorItem *item = new ColorItem;
//        item->setPos(::sin((i * 6.28) / 10.0) * 150,
//                     ::cos((i * 6.28) / 10.0) * 150);

//        scene.addItem(item);
//    }
//    QGraphicsView view(&scene);
//    view.setRenderHint(QPainter::Antialiasing);
//    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
//    view.setBackgroundBrush(QColor(230, 200, 167));
//    view.setWindowTitle("Desktop");
//    view.show();

    return a.exec();
}
