#include "ui//mainwindow.h"

#include <QtDebug>
#include <QApplication>
#include <ui/fragmentui.h>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <ui//storearea.h>
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

void showMat(cv::Mat m) {
    for (int i = 0; i < m.rows; ++i) {
        QString res = "";
        for (int j = 0; j < m.cols; ++j) {
            if (m.type() == CV_8UC1)
                res += QString::number(uchar(m.at<uchar>(i ,j))) + " ";
            else
                res += QString::number(float(m.at<float>(i ,j))) + " ";
        }
        qInfo() << res;
    }
}

int main(int argc, char *argv[])
{
//    cv::Mat m1 = cv::Mat::eye(2, 2, CV_8UC1);
//    m1.at<uchar>(0, 0) = 1;
//    m1.at<uchar>(0, 1) = 2;
//    m1.at<uchar>(1, 0) = 3;
//    m1.at<uchar>(1, 1) = 4;

//    cv::Mat trans = cv::Mat::eye(2, 3, CV_32FC1);
//    trans.at<float>(0, 0) = 1;
//    trans.at<float>(0, 1) = 2;
//    trans.at<float>(0, 2) = 0;
//    trans.at<float>(1, 0) = 2;
//    trans.at<float>(1, 1) = 4;
//    trans.at<float>(1, 2) = 0;
//    qInfo() << "--------------trans--------------------------";
//    showMat(trans);
//    qInfo() << "--------------before----------------";
//    showMat(m1);
//    qInfo() << "-------------after-----------------";
//    cv::warpAffine(m1, m1, trans, cv::Size(9, 9));
//    showMat(m1);
//    return 0;
    QApplication a(argc, argv);
    setStyle();
    loadLanguage();
    MainWindow w;
//    w.showMaximized();
//    w.showFullScreen();
    w.show();
    return a.exec();
}
