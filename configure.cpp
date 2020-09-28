#include "configure.h"
#include <QFile>
#include <QMessageBox>
#include <Tool.h>

QString Configure::readFromConfigure(const QString &name)
{
    QFile configFile("config.txt");
    if (!configFile.exists())
    {
        QMessageBox::warning(nullptr, QObject::tr("file error!"), QObject::tr("config file not exist!"),
                             QMessageBox::Cancel);
        return "error";
    }
    else if (!configFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr, QObject::tr("read error!"), QObject::tr("config file can't read!"),
                             QMessageBox::Cancel);
        return "error";
    }

    QJsonObject config = Tool::stringToJsonObj(configFile.readAll());
    QString res = config[name].toString();
    configFile.close();

    return res;
}

void Configure::writeToConfigure(const QString &name, const QString &value)
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
    config[name] = value;

    configFile.close();
    configFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    configFile.write(Tool::jsonObjToString(config).toUtf8());
    configFile.close();
    QMessageBox::information(nullptr, QObject::tr("language"), QObject::tr("set new language, please restart."),
                             QMessageBox::Ok);
}

Configure::Configure()
{

}
