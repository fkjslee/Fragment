#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QString>

class Configure
{
public:
    static QString readFromConfigure(const QString &name);

    static void writeToConfigure(const QString &name, const QString &value);

private:
    Configure();
};

#endif // CONFIGURE_H
