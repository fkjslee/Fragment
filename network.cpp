#include "network.h"
#include <WinSock2.h>
#include <vector>
#include <QtDebug>
#pragma comment(lib, "ws2_32.lib")

const QString helloMsg = "client: \"ping...\"";
const QString endMsg = "bye";

Network::Network()
{
}

namespace
{
    sockaddr_in initSockAddr(const char *addr, unsigned short port)
    {
        sockaddr_in sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));				 //每个字节都用0填充
        sockAddr.sin_family = PF_INET;
        sockAddr.sin_port = htons(port);
        sockAddr.sin_addr.s_addr = inet_addr(addr);
        return sockAddr;
    }
}


QString Network::sendMsg(const QString &msg)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in sockAddr = initSockAddr("166.111.139.116", 12345);
    connect(sock, reinterpret_cast<SOCKADDR *>(&sockAddr), sizeof(SOCKADDR));
    qInfo() << "Client send msg: " << msg;
    char szBuffer[2000] = { 0 };
    recv(sock, szBuffer, 2000, NULL);

    std::vector<QString> sendMsgs;
//    sendMsgs.push_back(helloMsg);
    sendMsgs.push_back(msg);
    sendMsgs.push_back(endMsg);
    QString res;
    for (QString sendMsg : sendMsgs)
    {
        int send_len = send(sock, sendMsg.toStdString().c_str(), sendMsg.length(), 0);
        if (send_len < 0)
        {
            qWarning() << "client send msg " + msg + " failed!";
            return "-1";
        }

        if (sendMsg == endMsg)
            break;
        recv(sock, szBuffer, 2000, NULL);
        res = szBuffer;
    }

    qInfo() << "server return msg : " << res;
    closesocket(sock);
    WSACleanup();
    return res;
}
