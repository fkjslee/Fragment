#include "ui//mainwindow.h"

#include <QtDebug>
#include <QApplication>
#include <ui/fragmentui.h>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <ui//desktop.h>
#include <QTranslator>
#include <QJsonObject>
#include <QJsonDocument>
#include <Tool.h>
#include <QMessageBox>
#include <QFile>
#include <network.h>

void setStyle()
{
    QApplication::setWindowIcon(QIcon(":/new/pre/resources/fragment.png"));
    QApplication::setStyle("fusion");
}

void loadLanguage()
{
    QFile configFile("config.txt");
    if (!configFile.exists())
    {
        QMessageBox::warning(nullptr, QObject::tr("file error!"), QObject::tr("config file not exist!"),
                             QMessageBox::Cancel);
        return;
    }
    else if (!configFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr, QObject::tr("read error!"), QObject::tr("config file can't read!"),
                             QMessageBox::Cancel);
        return;
    }

    QJsonObject config = Tool::stringToJsonObj(configFile.readAll());

    if (config["language"] == "CHS")
    {
        QTranslator *translator = new QTranslator();
        translator->load("translator_cn.qm");
        qApp->installTranslator(translator);
    }
    configFile.close();
}

int main(int argc, char *argv[])
{
    QString msg = "2 [[  0.57357712   0.81915217 214.88357691]\n [ -0.81915217   0.57357712  84.31745408]\n [  0.           0.           1.        ]] 3 [[ 2.58818590e-01 -9.65925703e-01  2.66034619e+02]\n [ 9.65925703e-01  2.58818590e-01 -1.57726320e+02]\n [ 0.00000000e+00  0.00000000e+00  1.00000000e+00]] 8 [[ -0.6946582    0.71934005 -14.85097284]\n [ -0.71934005  -0.6946582  494.63528587]\n [  0.           0.           1.        ]] ";
    msg.replace("[", "");
    msg.replace("]", "");
    msg.replace("\n", "");
    qDebug() << "msg = " << msg;
    QStringList msgList = msg.split(" ");
    QStringList msgList2;
    for (QString s : msgList)
        if (s != "")
            msgList2.append(s);
//    for (int i = 0; i < 10; ++i)
//        qDebug() << msgList2[i].toDouble();
    QApplication a(argc, argv);
    setStyle();
    loadLanguage();
    MainWindow w;
//    w.showMaximized();
//    w.showFullScreen();
    w.show();
    return a.exec();
}
