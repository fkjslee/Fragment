#ifdef _WIN32
#include "network.h"
#include <WinSock2.h>
#include <vector>
#include <QtDebug>
#include <QFile>
#include <Tool.h>
#include <windows.h>
#include <QDir>
#pragma comment(lib, "ws2_32.lib")

const QString helloMsg = "client: \"ping...\"";
const QString endMsg = "bye";

Network* Network::network = new Network;
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

#ifdef LOCAL_DATA

QString Network::sendMsg(const QString &msg) {
//    Sleep(5000);
    qInfo() << "Client send msg: " << msg;
    QString res;
    QStringList msgs = msg.split(' ');
    QString command = msgs[0];
    if (command == 'a') {
        int id1 = msgs[1].toInt();
        for (int i = 0; i < MAX_N; ++i) {
            if (network->allTransMat[id1][i].rows) {
                res += QString::number(i) + " ";
                for (int u = 0; u < 3; ++u)
                    for (int v = 0; v < 3; ++v) {
                        float num = float(network->allTransMat[id1][i].at<float>(u, v));
                        res += QString::number(num) + " ";
                    }
            }
        }
        return res;
    } else if (command == 'b') {
        int id1 = msgs[1].toInt();
        int id2 = msgs[2].toInt();
        if (network->allTransMat[id1][id2].rows) {
            for (int u = 0; u < 3; ++u)
                for (int v = 0; v < 3; ++v) {
                    float num = float(network->allTransMat[id1][id2].at<float>(u, v));
                    res += QString::number(num) + " ";
            }
        } else {
            res = "These two fragments are not aligned.";
        }
        return res;
    } else {
        return "wrong command";
    }
}

void Network::loadTransMat(const QString &path)
{
    QFile transMatPath(path + QDir::separator() + "pairwise_alignment.txt");
    transMatPath.open(QIODevice::ReadOnly);
    QTextStream in(&transMatPath);
    QString line = in.readLine();
    QStringList lines;
    while (!line.isNull())
    {
        lines.append(line);
        line = in.readLine();
    }
    for (int i = 0; i < lines.length(); i += 4) {
        QStringList ids = lines[i].split('\t');
        int id1 = ids[0].toInt();
        int id2 = ids[1].toInt();
        QStringList m1 = lines[i+1].split(' ');
        QStringList m2 = lines[i+2].split(' ');
        cv::Mat mat = cv::Mat::eye(3, 3, CV_32FC1);
        mat.at<float>(0, 0) = m1[0].toFloat();
        mat.at<float>(0, 1) = m1[1].toFloat();
        mat.at<float>(0, 2) = m1[2].toFloat();
        mat.at<float>(1, 0) = m2[0].toFloat();
        mat.at<float>(1, 1) = m2[1].toFloat();
        mat.at<float>(1, 2) = m2[2].toFloat();
        network->allTransMat[id1][id2] = mat.clone();
        cv::invert(mat, mat);
        network->allTransMat[id2][id1] = mat.clone();
    }
}


#else
QString Network::sendMsg(const QString &msg)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
//    int nNetTimeout = 10000;
//    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, ( char * )&nNetTimeout, sizeof(int));

    sockaddr_in sockAddr = initSockAddr("166.111.139.116", 12345);
    connect(sock, reinterpret_cast<SOCKADDR *>(&sockAddr), sizeof(SOCKADDR));
    qInfo() << "Client send msg: " << msg;
    char szBuffer[2000] = { 0 };
    recv(sock, szBuffer, 2000, NULL);

    std::vector<QString> sendMsgs;
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

    qInfo() << "server return msg : " << res.replace("/n", "");
    closesocket(sock);
    WSACleanup();
    return res;
}
#endif


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
    serv_addr.sin_addr.s_addr = inet_addr("166.111.139.116");
    serv_addr.sin_port = htons(12345); //port
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    const int recvLen = 4096;
    char szBuffer[recvLen];
    if (recv(sock, szBuffer, recvLen, 0) == -1) {
        qCritical() << "can't connect to server!!!!!!!!!!";
        return "-1";
    }


    qInfo() << "client send msg : " << msg.left(20);
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
        recv(sock, szBuffer, recvLen, 0);
        res = szBuffer;
    }

    qInfo() << "server return msg : " << res;

    //关闭套接字
    close(sock);
    return res;
}

#endif
