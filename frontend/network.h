#ifndef NETWORK_H
#define NETWORK_H

#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>

#define LOCAL_DATA

const int MAX_FRAGMENT_NUM = 500;

class TransMatAndConfi {
public:
    int thisFrag;
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
    std::vector<TransMatAndConfi> allTransMat[MAX_FRAGMENT_NUM];
    static Network* network;

    static int fragSuggesNum;
    static float fragSuggesConfi;

private:
    Network() {
    }
};

#endif // NETWORK_H

