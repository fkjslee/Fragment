#ifndef TOOL_H
#define TOOL_H

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QtDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUndoStack>
#include "opencv2/imgcodecs/legacy/constants_c.h"
#include <opencv2/imgproc/types_c.h>
#include <math.h>
#include <qrgb.h>

#define INFINITE 0x3f3f3f3f

using std::vector;

namespace
{
}

class Tool
{
public:

    static QString jsonObjToString(const QJsonObject &json)
    {
        return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
    }

    static void showMat(const cv::Mat& src) {
        if (src.type() != CV_32FC1) std::cout << "wrong type " << std::endl;
        else if (src.rows * src.cols != 9 && src.rows * src.cols != 6) std::cout << "wrong size" << std::endl;
        else {
            for (int i = 0; i < src.rows; ++i) {
                QString res = "";
                for (int j = 0; j < src.cols; ++j)
                    res += QString::number(double(src.at<float>(i, j))) + "    ";
                qInfo() << res;
            }
        }
    }

    static cv::Mat getFirst2RowsMat(const cv::Mat& src) {
        if (src.type() != CV_32FC1) return cv::Mat(0, 0, CV_32FC1);
        if (src.size() != cv::Size(3, 3)) return cv::Mat(0, 0, CV_32FC1);
        if (abs(src.at<float>(2, 0)) > 1e-7 || abs(src.at<float>(2, 1)) > 1e-7 || abs(src.at<float>(2, 2) - 1 > 1e-7))
            return cv::Mat(0, 0, CV_32FC1);
        cv::Mat dst(2, 3, CV_32FC1);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j)
                dst.at<float>(i, j) = src.at<float>(i, j);
        }
        return dst;
    }

    static cv::Mat getFirst3RowsMat(const cv::Mat& src) {
        if (src.type() != CV_32FC1) return cv::Mat(0, 0, CV_32FC1);
        if (src.size() != cv::Size(2, 3)) return cv::Mat(0, 0, CV_32FC1);
        cv::Mat dst(3, 3, CV_32FC1);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j)
                dst.at<float>(i, j) = src.at<float>(i, j);
        }
        dst.at<float>(2, 0) = dst.at<float>(2, 1) = 0;
        dst.at<float>(2, 2) = 1.0;
        return dst;
    }

    static QJsonObject stringToJsonObj(const QString &str)
    {
        QJsonObject json;
        QJsonParseError err;
        QJsonDocument l_doc = QJsonDocument::fromJson(str.toUtf8(), &err);
        if (err.error == QJsonParseError::NoError)
        {
            if (l_doc.isObject())
            {
                json = l_doc.object();
            }
        }
        return json;
    }

    static void *ucharToVoid(const uchar *c)
    {
        return reinterpret_cast<void *>(const_cast<uchar *>(c));
    }

    static cv::Mat str2TransMat(QString src) {
        cv::Mat dst(3, 3, CV_32FC1);
        if (src == "-1" || src == "These two fragments are not aligned.") {
            return cv::Mat(0, 0, CV_32FC1);
        }
        src.replace("[", "");
        src.replace("]", "");
        src.replace("\n", "");
        QStringList msgList = src.split(" ");
        QStringList msgList2;
        for (QString s : msgList)
            if (s != "")
                msgList2.append(s);

        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                dst.at<float>(j, k) = msgList2[j*3+k].toFloat();
        return dst;
    }

    static cv::Mat QImageToMat(const QImage &image)
    {
        cv::Mat mat;
        switch(image.format())
        {
            case QImage::Format_ARGB32:
            case QImage::Format_RGB32:
            case QImage::Format_ARGB32_Premultiplied:
                mat = cv::Mat(image.height(), image.width(), CV_8UC4, ucharToVoid(image.constBits()), size_t(image.bytesPerLine()));
//            reinterpret_cast<void*>(image.constBits())
                mat = cv::Mat(image.height(), image.width(), CV_8UC4, ucharToVoid(image.constBits()), size_t(image.bytesPerLine()));
                break;
            case QImage::Format_RGB888:
                mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
                cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
                break;
            case QImage::Format_Indexed8:
                mat = cv::Mat(image.height(), image.width(), CV_8UC1, ucharToVoid(image.constBits()), size_t(image.bytesPerLine()));
                break;
            default:
                ;
        }
        return mat;
    }

    static cv::Mat Mat8UC4To8UC3(const cv::Mat& src) {
        cv::Mat dst(src.size(), CV_8UC3);
        for (int i = 0; i < src.rows; ++i)
            for (int j = 0; j < src.cols; ++j) {
                for (int channel = 0; channel < 3; ++channel)
                    dst.at<cv::Vec3b>(i, j)[channel] = src.at<cv::Vec4b>(i, j)[channel];
            }
        return dst;
    }

    static cv::Mat Mat8UC3To8UC4(const cv::Mat& src) {
        cv::Mat dst(src.size(), CV_8UC4);
        for (int i = 0; i < src.rows; ++i)
            for (int j = 0; j < src.cols; ++j) {
                for (int channel = 0; channel < 3; ++channel)
                    dst.at<cv::Vec4b>(i, j)[channel] = src.at<cv::Vec3b>(i, j)[channel];
                dst.at<cv::Vec4b>(i, j)[3] = 0xff;
            }
        return dst;
    }

    static QImage MatToQImage(const cv::Mat &mat)
    {
        // 8-bits unsigned, NO. OF CHANNELS = 1
        if(mat.type() == CV_8UC1)
        {
            QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
            // Set the color table (used to translate colour indexes to qRgb values)
            image.setColorCount(256);
            for(int i = 0; i < 256; i++)
            {
                image.setColor(i, qRgb(i, i, i));
            }
            // Copy input Mat
            uchar *pSrc = mat.data;
            for(int row = 0; row < mat.rows; row ++)
            {
                uchar *pDest = image.scanLine(row);
                memcpy(pDest, pSrc, size_t(mat.cols));
                pSrc += mat.step;
            }
            return image;
        }
        // 8-bits unsigned, NO. OF CHANNELS = 3
        else if(mat.type() == CV_8UC3)
        {
            // Copy input Mat
            const uchar *pSrc = static_cast<const uchar *>(mat.data);
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, int(mat.step), QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        else if(mat.type() == CV_8UC4)
        {
            const uchar *pSrc = static_cast<const uchar *>(mat.data);
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, int(mat.step), QImage::Format_RGB32);
            auto mask = image.createMaskFromColor(qRgb(0, 0, 0), Qt::MaskMode::MaskOutColor);
            image.setAlphaChannel(mask);
            return image.copy();
        }
        else
        {
            qCritical() << "ERROR: Mat could not be converted to QImage.";
            return QImage();
        }
    }

    template<typename T>
    static typename::vector<T>::const_iterator findInVector(const vector<T> &vec, const T &value)
    {
        return std::find(vec.begin(), vec.end(), value);
    }

    template<typename T>
    static void eraseInVector(vector<T> &vec, const T &value)
    {
        vec.erase(findInVector(vec, value));
    }
};

#endif // TOOL_H
