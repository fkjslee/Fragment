#ifndef NETWORK_H
#define NETWORK_H

#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>

#define LOCAL_DATA

const int MAX_N = 500;

class TransMatAndConfi {
public:
    int otherFrag;
    cv::Mat transMat;
    float confidence;
    bool operator < (const TransMatAndConfi& rhs) const {
        return this->confidence > rhs.confidence;
    }
};

class Network
{
public:
    static QString sendMsg(const QString &msg);
    static void loadTransMat(const QString& path);
    std::vector<TransMatAndConfi> allTransMat[MAX_N];
    static Network* network;

    static int fragSuggesNum;

private:
    Network() {
    }
};

#endif // NETWORK_H

