#ifdef _WIN32
#include "network.h"
#include <vector>
#include <QtDebug>
#include<netinet/in.h>
//#pragma comment(lib, "ws2_32.lib")

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


#else
#include "network.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>
#include <QtDebug>

const QString helloMsg = "client: \"ping...\"";
const QString endMsg = "bye";
namespace {
using namespace std;
}

Network::Network()
{
}

QString Network::sendMsg(const QString &msg)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("166.111.139.116");  //具体的IP地址
    serv_addr.sin_port = htons(12345);  //端口
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char szBuffer[2000];
    recv(sock, szBuffer, 2000, NULL);


    qInfo() << "client send msg : " << msg;
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

    //关闭套接字
    close(sock);
    return res;
}

#endif
