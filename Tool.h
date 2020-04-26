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

    static cv::Mat stringToMat(const std::string& src) {
        cv::Mat mat2(1, src.size(), CV_8U, (char*)src.data());
        cv::Mat dst = cv::imdecode(mat2, CV_LOAD_IMAGE_COLOR);
        return dst;
    }

    static std::string matToString(const cv::Mat& src) {
        std::string dst;
        std::vector<unsigned char> buff;
        cv::imencode(".png", src, buff);
        dst.resize(buff.size());
        memcpy(&dst[0], buff.data(), buff.size());
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
            return image.copy();
        }
        else
        {
            qCritical() << "ERROR: Mat could not be converted to QImage.";
            return QImage();
        }
    }

    static double calcLeftRightAbsGrayscale(const cv::Mat &left, const cv::Mat &rhs)
    {
        double res = 0;
        if (left.type() != rhs.type() || left.rows != rhs.rows || left.type() != CV_8UC4)
            return INFINITE;
        int c = left.cols - 1;
        for (int r = 0; r < left.rows; ++r)
        {
            for (int channel = 0; channel < 4; ++channel)
                res += abs(left.at<cv::Vec4b>(r, c)[channel] - rhs.at<cv::Vec4b>(r, 0)[channel]);
        }
        return res / left.rows;
    }

    static double calcUpDownAbsGrayscale(const cv::Mat &up, const cv::Mat &down)
    {
        double res = 0;
        if (up.type() != down.type() || up.cols != down.cols || up.type() != CV_8UC4)
            return INFINITE;
        int r = up.rows - 1;
        for (int c = 0; c < up.cols; ++c)
        {
            for (int channel = 0; channel < 4; ++channel)
                res += abs(up.at<cv::Vec4b>(r, c)[channel] - down.at<cv::Vec4b>(0, c)[channel]);
        }
        return res / up.cols;
    }

    static double calcSimilarity(const cv::Mat &m1, const cv::Mat &m2)
    {
        if (m1.type() != CV_8UC4 || m2.type() != CV_8UC4 || m1.size != m2.size)
            return INFINITE;
        double res = 0;

        for (int r = 0; r < m1.rows; ++r)
        {
            for (int c = 0; c < m1.cols; ++c)
            {
                for (int channel = 0; channel < 4; ++channel)
                {
                    res += abs(m1.at<cv::Vec4b>(r, c)[channel] - m2.at<cv::Vec4b>(r, c)[channel]);
                }
            }
        }
        return res / (m1.rows * m1.cols * m1.channels());
    }

    static QString aHash(const cv::Mat &cmpImg)
    {
        cv::Mat img = cmpImg.clone();
        cv::resize(img, img, cv::Size(8, 8));
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        double absGrayscale = 0;
        for (int r = 0; r < img.rows; ++r)
        {
            for (int c = 0; c < img.cols; ++c)
            {
                absGrayscale += img.at<uchar>(r, c);
            }
        }
        QString hashString = "";
        absGrayscale /= (img.rows * img.cols);
        for (int r = 0; r < img.rows; ++r)
        {
            for (int c = 0; c < img.cols; ++c)
            {
                if (img.at<uchar>(r, c) > absGrayscale)
                    hashString += "1";
                else
                    hashString += "0";
            }
        }
        return hashString;
    }

    // from blog https://blog.csdn.net/enter89/article/details/90293971
    static int cmpWithOriginalMat(const cv::Mat &m1, const cv::Mat &m2)
    {
        const QString &h1 = aHash(m1);
        const QString &h2 = aHash(m2);
        int res = 0;
        if (h1.size() != h2.size())
            return -1;
        for (int i = 0; i < h1.size(); ++i)
            if (h1[i] != h2[i])
                ++res;
        return res;
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
