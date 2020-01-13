#include "mainwindow.h"

#include <QtDebug>
#include <QApplication>
#include <fragment.h>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <desktop.h>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/new/pre/resources/fragment.png"));
    QApplication::setStyle("fusion");
    QTranslator* translator = new QTranslator();
    translator->load("translator_cn.qm");
    qApp->installTranslator(translator);
    qDebug() << QObject::tr("hello world");
    MainWindow w;
//    w.showMaximized();
//    w.showFullScreen();
    w.show();
    return a.exec();
}
