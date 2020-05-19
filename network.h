#ifndef NETWORK_H
#define NETWORK_H

#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>

#define LOCAL_DATA

const int MAX_N = 500;

class Network
{
public:
    static QString sendMsg(const QString &msg);
    static void loadTransMat(const QString& path);
    cv::Mat allTransMat[MAX_N][MAX_N];
    static Network* network;

private:
    Network() {
    }
};

#endif // NETWORK_H

