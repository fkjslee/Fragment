#ifdef _WIN32
#include "network.h"
#include <WinSock2.h>
#include <vector>
#include <QtDebug>
#include <QFile>
#include <Tool.h>
#include <windows.h>
#include <algorithm>
#include <QDir>
#pragma comment(lib, "ws2_32.lib")

const QString helloMsg = "client: \"ping...\"";
const QString endMsg = "bye";

int Network::fragSuggesNum = 200;
float Network::fragSuggesConfi = 0.8;
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

    template<class T>
    const T& minElement(const T& a, const T& b)
    {
        return (b < a) ? b : a;
    }

    template<class T>
    const T& maxElement(const T& a, const T& b)
    {
        return (b < a) ? a : b;
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
        for (int i = 0; i < minElement(fragSuggesNum, (int)network->allTransMat[id1].size()); ++i) {
            int id2 = network->allTransMat[id1][i].otherFrag;
            res += QString::number(id2) + " ";
            res += QString::number(network->allTransMat[id1][i].confidence) + " ";
            for (int u = 0; u < 3; ++u)
                for (int v = 0; v < 3; ++v) {
                    float num = float(network->allTransMat[id1][i].transMat.at<float>(u, v));
                    res += QString::number(num) + " ";
                }
        }
        return res;
    } else if (command == 'b') {
        // don't need
        return "wrong command";
    } else {
        return "wrong command";
    }
}

void Network::loadTransMat(const QString &path)
{
    QDir dir(path);
    QStringList filter;
    filter << "*_calc_res_np_res.txt";
    QStringList nameList = dir.entryList(filter);


    QFile transMatPath(path + QDir::separator() + nameList[0]);
    transMatPath.open(QIODevice::ReadOnly);
    QTextStream in(&transMatPath);
    QString line = in.readLine();
    QStringList lines;
    while (!line.isNull())
    {
        lines.append(line);
        line = in.readLine();
    }
    for (int i = 0; i < MAX_FRAGMENT_NUM; ++i)
        network->allTransMat[i].clear();
    for (int i = 0; i < lines.length(); i += 4) {
        QStringList ids = lines[i].split(' ');
        int id1 = int(ids[0].toFloat() + 0.5);
        int id2 = int(ids[1].toFloat() + 0.5);
        float confidence = ids[2].toFloat();
        QStringList m1 = lines[i+1].split(' ');
        QStringList m2 = lines[i+2].split(' ');
        cv::Mat mat = cv::Mat::eye(3, 3, CV_32FC1);
        mat.at<float>(0, 0) = m1[0].toFloat();
        mat.at<float>(0, 1) = m1[1].toFloat();
        mat.at<float>(0, 2) = m1[2].toFloat();
        mat.at<float>(1, 0) = m2[0].toFloat();
        mat.at<float>(1, 1) = m2[1].toFloat();
        mat.at<float>(1, 2) = m2[2].toFloat();
        TransMatAndConfi matAndConfi;
        matAndConfi.otherFrag = id2;
        matAndConfi.transMat = mat.clone();
        matAndConfi.confidence = confidence;
        bool has = false;
        for (TransMatAndConfi m : network->allTransMat[id1]) {
            if (m.otherFrag == id2)
                has = true;
        }
        if (!has)
            network->allTransMat[id1].emplace_back(matAndConfi);

        matAndConfi.otherFrag = id1;
        matAndConfi.transMat = Tool::getInvMat(mat.clone());
        matAndConfi.confidence = confidence;
        has = false;
        for (TransMatAndConfi m : network->allTransMat[id2]) {
            if (m.otherFrag == id1)
                has = true;
        }
        if (!has)
            network->allTransMat[id2].emplace_back(matAndConfi);
    }

    for (int i = 0; i < MAX_FRAGMENT_NUM; ++i)
        std::sort(network->allTransMat[i].begin(), network->allTransMat[i].end());

    float minConfi = 1000000.0;
    float maxConfi = -1000000.0;
    for (int i = 0; i < MAX_FRAGMENT_NUM; ++i) {
        for (int j = 0; j < (int)network->allTransMat[i].size(); ++j) {
            minConfi = minElement(minConfi, network->allTransMat[i][j].confidence);
            maxConfi = maxElement(maxConfi, network->allTransMat[i][j].confidence);
        }
    }
    for (int i = 0; i < MAX_FRAGMENT_NUM; ++i) {
        for (int j = 0; j < (int)network->allTransMat[i].size(); ++j) {
            network->allTransMat[i][j].confidence = (network->allTransMat[i][j].confidence - minConfi) / (maxConfi - minConfi);
//            qInfo() << i << network->allTransMat[i][j].otherFrag << " = " << network->allTransMat[i][j].confidence;
        }
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
