#ifndef NETWORK_H
#define NETWORK_H

#include <QString>

class Network
{
public:
    Network();
    static QString sendMsg(const QString &msg);
};

#endif // NETWORK_H

