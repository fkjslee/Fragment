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
#include <QDateTime>
#include <network.h>
#include <ui/refreshthread.h>
#include <QMutex>
#include <configure.h>

void setStyle()
{
    QApplication::setWindowIcon(QIcon(":/new/pre/resources/fragment.png"));
    QApplication::setStyle("fusion");
}

void loadLanguage()
{
    if (Configure::readFromConfigure("language") == "CHS")
    {
        QTranslator *translator = new QTranslator();
        translator->load("translator_cn.qm");
        qApp->installTranslator(translator);
    }
}

QString logFilePath = "";
QFile *logFile = nullptr;
QMutex locker;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString textType;
    switch (type)
    {
        case QtDebugMsg:
            textType = QString("Debug");
            break;
        case QtWarningMsg:
            textType = QString("Warning");
            break;
        case QtCriticalMsg:
            textType = QString("Critical");
            break;
        case QtFatalMsg:
            textType = QString("Fatal");
            break;
        default:
            break;
    }

    locker.lock();
    QString text;
    text = QString("[%1] %2: %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(textType).arg(msg);
    fprintf(stderr, "%s\n", text.toStdString().c_str());
    if (logFile == nullptr)
    {
        int cnt = 0;
        logFilePath = "./log/log_file_" + QString::number(cnt) + ".txt";
        logFile = new QFile(logFilePath);
        while(QFile(logFilePath).exists())
        {
            ++cnt;
            logFilePath = "./log/log_file_" + QString::number(cnt) + ".txt";
            logFile = new QFile(logFilePath);
        }
        fprintf(stderr, "log file path : %s\n", logFilePath.toStdString().c_str());
    }
    QTextStream text_stream(logFile);
    logFile->open(QIODevice::ReadWrite | QIODevice::Append);
    text_stream << text << endl;
    logFile->flush();
    logFile->close();
    locker.unlock();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    qInfo() << "launch application. delay =" << Network::delay << "ms confidence =" << RefreshThread::confidence << "%";
    QApplication a(argc, argv);
    setStyle();
    loadLanguage();
    MainWindow w;
    Configure::readFromConfigure("language");
//    w.showFullScreen();
    w.show();
    return a.exec();

//    cv::Mat m1 = cv::imread("./mixed2/fragment_0008.png");
//    cv::Mat m1_gauss;
//    cv::GaussianBlur(m1, m1_gauss, cv::Size(3, 3), 0);
//    cv::Mat edges;
//    cv::Canny(m1_gauss, edges, 50, 150, 3);
//    cv::imshow("fffs", edges);
//    cv::Mat lines;
//    qInfo() << edges.type() << CV_32FC2 << CV_32SC2;
//    cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 50, 50, 10);
//    std::vector<int> angs;
//    for (int i = 0; i < lines.rows; ++i)
//    {
//        for (int j = 0; j < lines.cols; ++j)
//        {
//            cv::Vec4i line = lines.at<cv::Vec4i>(i, j);
//            if (line[2] - line[0] == 0) angs.push_back(180);
//            else
//            {
//                double rad = std::atan((line[3] - line[1]) / (line[2] - line[0]));
//                angs.push_back(rad * 180 / CV_PI);
//            }
//            cv::line(m1, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(0, 255, 0), 2);
//        }
//    }
//    cv::imshow("fff", m1);
//    cv::waitKey(0);
}
