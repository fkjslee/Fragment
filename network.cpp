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
int Network::delay = 1 * 1000;
Network* Network::network = new Network;
namespace
{
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

QString Network::sendMsg(const QString &msg) {
    qInfo() << "Client send msg: " << msg;
    Sleep(delay);
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

