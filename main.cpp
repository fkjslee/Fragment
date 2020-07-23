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

QString logFilePath = "";
QFile* logFile = nullptr;
QMutex locker;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString textType;
    switch (type) {
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
    if (logFile == nullptr) {
        int cnt = 0;
        logFilePath = "./log/log_file_" + QString::number(cnt) + ".txt";
        logFile = new QFile(logFilePath);
        while(QFile(logFilePath).exists()) {
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
//    w.showFullScreen();
    w.show();
    return a.exec();
}
