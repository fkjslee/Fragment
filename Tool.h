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
#include <fragmentscontroller.h>
#include <ui/fragmentui.h>

#define _USE_MATH_DEFINES

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

    static void showMat(const cv::Mat &src)
    {
        if (src.type() != CV_32FC1 && src.type() != CV_64FC1) std::cout << "wrong type " << std::endl;
        else if (src.rows * src.cols != 9 && src.rows * src.cols != 6) std::cout << "wrong size" << std::endl;
        else
        {
            for (int i = 0; i < src.rows; ++i)
            {
                QString res = "";
                for (int j = 0; j < src.cols; ++j)
                    res += QString::number(double(src.at<float>(i, j))) + "    ";
                qInfo() << res;
            }
        }
    }

    static cv::Mat getFirst2RowsMat(const cv::Mat &src)
    {
        if (src.type() != CV_32FC1 && src.type() != CV_64FC1) return cv::Mat(0, 0, CV_32FC1);
        if (src.size() != cv::Size(3, 3)) return cv::Mat(0, 0, CV_32FC1);
        if (abs(src.at<float>(2, 0)) > 1e-7 || abs(src.at<float>(2, 1)) > 1e-7 || abs(src.at<float>(2, 2) - 1.0) > 1e-7)
            return cv::Mat(0, 0, CV_32FC1);
        cv::Mat dst(2, 3, CV_32FC1);
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 3; ++j)
                dst.at<float>(i, j) = src.at<float>(i, j);
        }
        return dst.clone();
    }

    static cv::Mat getFirst3RowsMat(const cv::Mat &src)
    {
        if (src.type() != CV_32FC1 && src.type() != CV_64FC1) return cv::Mat(0, 0, CV_32FC1);
        if (src.size() != cv::Size(3, 2)) return cv::Mat(0, 0, CV_32FC1);
        cv::Mat dst(3, 3, CV_32FC1);
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 3; ++j)
                dst.at<float>(i, j) = src.at<float>(i, j);
        }
        dst.at<float>(2, 0) = dst.at<float>(2, 1) = 0;
        dst.at<float>(2, 2) = 1.0;
        return dst.clone();
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

    static bool checkFragInFragmentArea(FragmentUi *frag)
    {
        bool exist = false;
        for (FragmentUi *f : FragmentsController::getController()->getUnsortedFragments())
        {
            if (f == frag)
                exist = true;
        }
        return exist;
    }

    static cv::Mat str2TransMat(QString src)
    {
        cv::Mat dst(3, 3, CV_32FC1);
        if (src == "-1" || src == "These two fragments are not aligned.")
        {
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
                dst.at<float>(j, k) = msgList2[j * 3 + k].toFloat();
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
                mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void *)image.constBits(), image.bytesPerLine());
                cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
                break;
            case QImage::Format_Indexed8:
                mat = cv::Mat(image.height(), image.width(), CV_8UC1, ucharToVoid(image.constBits()), size_t(image.bytesPerLine()));
                break;
            default:
                ;
        }
        return mat.clone();
    }

    static cv::Mat Mat8UC4To8UC3(const cv::Mat &src)
    {
        cv::Mat dst(src.size(), CV_8UC3);
        for (int i = 0; i < src.rows; ++i)
            for (int j = 0; j < src.cols; ++j)
            {
                for (int channel = 0; channel < 3; ++channel)
                    dst.at<cv::Vec3b>(i, j)[channel] = src.at<cv::Vec4b>(i, j)[channel];
            }
        return dst;
    }

    static cv::Mat Mat8UC3To8UC4(const cv::Mat &src)
    {
        cv::Mat dst(src.size(), CV_8UC4);
        for (int i = 0; i < src.rows; ++i)
            for (int j = 0; j < src.cols; ++j)
            {
                for (int channel = 0; channel < 3; ++channel)
                    dst.at<cv::Vec4b>(i, j)[channel] = src.at<cv::Vec3b>(i, j)[channel];
                dst.at<cv::Vec4b>(i, j)[3] = 0xff;
            }
        return dst;
    }

    static QImage Mat8UC4ToQImage(const cv::Mat &mat)
    {
        if(mat.type() == CV_8UC4)
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

    static float angToRad(int ang)
    {
        return 1.0 * ang / 180.0 * 3.141592653;
    }

    // cv::getRotationMatrix2D may get singular matrix
    static cv::Mat getRotationMatrix(float x, float y, float rad)
    {
        cv::Mat move = cv::Mat::eye(3, 3, CV_32FC1);
        move.at<float>(0, 2) = x;
        move.at<float>(1, 2) = y;
        cv::Mat rotate = cv::Mat::eye(3, 3, CV_32FC1);
        rotate.at<float>(0, 0) = std::cos(rad);
        rotate.at<float>(0, 1) = -std::sin(rad);
        rotate.at<float>(1, 0) = std::sin(rad);
        rotate.at<float>(1, 1) = std::cos(rad);
        cv::Mat moveInv = move.clone();
        cv::invert(moveInv, moveInv);
        return getFirst2RowsMat(move * rotate * moveInv).clone();
    }

    static cv::Mat normalToOpencvTransMat(const cv::Mat &src)
    {
        cv::Mat dst = cv::Mat::eye(3, 3, CV_32FC1);
        dst.at<float>(0, 0) = src.at<float>(0, 0);
        dst.at<float>(0, 1) = src.at<float>(1, 0);
        dst.at<float>(0, 2) = src.at<float>(1, 2);
        dst.at<float>(1, 0) = src.at<float>(0, 1);
        dst.at<float>(1, 1) = src.at<float>(1, 1);
        dst.at<float>(1, 2) = src.at<float>(0, 2);
        return dst;
    }

    static cv::Mat opencvToNormalTransMat(const cv::Mat &src)
    {
        cv::Mat dst = cv::Mat::eye(3, 3, CV_32FC1);
        dst.at<float>(0, 0) = src.at<float>(0, 0);
        dst.at<float>(0, 1) = src.at<float>(1, 0);
        dst.at<float>(0, 2) = src.at<float>(1, 2);
        dst.at<float>(1, 0) = src.at<float>(0, 1);
        dst.at<float>(1, 1) = src.at<float>(1, 1);
        dst.at<float>(1, 2) = src.at<float>(0, 2);
        return dst;
    }

    static cv::Mat getInvMat(const cv::Mat &src)
    {
        cv::Mat dst = src.clone();
        cv::invert(src, dst);
        return dst;
    }

    static cv::Mat getOpencvMat(const cv::Mat src)
    {
        cv::Mat dst = cv::Mat::eye(2, 3, CV_32FC1);
        dst.at<float>(0, 0) = src.at<float>(0, 0);
        dst.at<float>(0, 1) = src.at<float>(1, 0);
        dst.at<float>(0, 2) = src.at<float>(1, 2);
        dst.at<float>(1, 0) = src.at<float>(0, 1);
        dst.at<float>(1, 1) = src.at<float>(1, 1);
        dst.at<float>(1, 2) = src.at<float>(0, 2);
        return dst.clone();
    }

    static cv::Mat getMatFromAngleAndOffset(int ang, int off_x, int off_y)
    {
        double rad = ang * CV_PI / 180;
        cv::Mat res = cv::Mat::eye(3, 3, CV_32FC1);
        res.at<float>(0, 0) = float(std::cos(rad));
        res.at<float>(0, 1) = -float(std::sin(rad));
        res.at<float>(0, 2) = float(off_x);
        res.at<float>(1, 0) = float(std::sin(rad));
        res.at<float>(1, 1) = float(std::cos(rad));
        res.at<float>(1, 2) = float(off_y);
        return res;
    }

    static int colorIdxCnt(const cv::Mat img, const cv::Point idx, int pixel = 1)
    {
        cv::Vec3b blackColor(0, 0, 0);
        int colorCnt = 0;
        for (int y = idx.y - pixel; y <= idx.y + pixel; ++y)
            for (int x = idx.x - pixel; x <= idx.x + pixel; ++x)
            {
                if (y <= 0 || y >= img.rows || x <= 0 || x >= img.cols) continue;
                if (img.at<cv::Vec3b>(y, x) != blackColor)
                {
                    if (std::abs(y - idx.y) == 0 && std::abs(x - idx.x) == 0) continue;
                    else if (std::abs(y - idx.y) < pixel && std::abs(x - idx.x) < pixel) continue;
                    else if (std::abs(y - idx.y) <= pixel && std::abs(x - idx.x) <= pixel) colorCnt++;
                }
            }
        return colorCnt;
    }

    static std::vector<cv::Point> getBoundIdx(const cv::Mat img)
    {
        cv::Vec3b blackColor(0, 0, 0);
        std::vector<cv::Point> boundIdx;
        for (int i = 0; i < img.rows; ++i)
            for (int j = 0; j < img.cols; ++j)
            {
                cv::Point idx = cv::Point(j, i);
                int colorCnt = colorIdxCnt(img, idx, 1);
                if (img.at<cv::Vec3b>(i, j) != blackColor && colorCnt < 8 && colorCnt > 0)
                {
                    boundIdx.push_back(idx);
                }
            }
        return boundIdx;
    }

    static void possFusionImage(const cv::Mat &src1, const cv::Mat &src2, int &ang, int &off_x, int &off_y)
    {
        cv::Mat offset;
        int fit_ang = ang;
        int fit_off_x = off_x;
        int fit_off_y = off_y;
        int fit_value = -1;
        for (int poss_ang = ang - 5; poss_ang <= ang + 5; ++poss_ang)
            for (int poss_x = off_x - 2; poss_x <= off_x + 2; ++poss_x)
                for (int poss_y = off_y - 2; poss_y <= off_y + 2; ++poss_y)
                {
                    cv::Mat poss_trans = getMatFromAngleAndOffset(poss_ang, poss_x, poss_y);

                    cv::Mat img_src1_src2 = fusionImage(src1, src2, poss_trans, offset);

                    cv::Mat after_src_1;
                    cv::warpAffine(src1, after_src_1, offset(cv::Range(0, 2), cv::Range(0, 3)), img_src1_src2.size());
                    cv::Mat after_src_2;
                    cv::warpAffine(src2, after_src_2, (offset * poss_trans)(cv::Range(0, 2), cv::Range(0, 3)), img_src1_src2.size());

                    std::vector<cv::Point> boundIdx_3 = getBoundIdx(after_src_1);
                    std::vector<cv::Point> boundIdx_5 = getBoundIdx(after_src_2);

                    std::vector<cv::Point> colorIdx;
                    cv::Vec3b blackColor(0, 0, 0);
                    for (cv::Point pt : boundIdx_3)
                    {
                        int p_cnt_5 = colorIdxCnt(after_src_2, pt, 1);
                        if (p_cnt_5 < 8 && p_cnt_5 > 0) colorIdx.emplace_back(pt);
                    }

                    for (cv::Point pt : colorIdx)
                    {
                        after_src_2.at<cv::Vec3b>(pt.y, pt.x) = cv::Vec3b(255, 0, 255);
                    }

                    if (int(colorIdx.size()) > fit_value)
                    {
                        fit_value = int(colorIdx.size());
                        fit_ang = poss_ang;
                        fit_off_x = poss_x;
                        fit_off_y = poss_y;
                    }
                }
        ang = fit_ang;
        off_x = fit_off_x;
        off_y = fit_off_y;
    }

    static cv::Mat fusionImage(const cv::Mat &src, const cv::Mat &dst, const cv::Mat &transMat, cv::Mat &offset)
    {
        std::vector<cv::Point2i> colorIdx;
        for (int i = 0; i < src.rows; ++i)
            for (int j = 0; j < src.cols; ++j)
            {
                int sum = src.at<cv::Vec4b>(i, j)[0] + src.at<cv::Vec4b>(i, j)[1] + src.at<cv::Vec4b>(i, j)[2];
                if (sum != 0)
                    colorIdx.push_back(cv::Point2i(j, i));
            }
        for (int i = 0; i < dst.rows; ++i)
            for (int j = 0; j < dst.cols; ++j)
            {
                int sum = dst.at<cv::Vec4b>(i, j)[0] + dst.at<cv::Vec4b>(i, j)[1] + dst.at<cv::Vec4b>(i, j)[2];
                if (sum != 0)
                {
                    int x = transMat.at<float>(0, 0) * j + transMat.at<float>(0, 1) * i + transMat.at<float>(0, 2);
                    int y = transMat.at<float>(1, 0) * j + transMat.at<float>(1, 1) * i + transMat.at<float>(1, 2);
                    colorIdx.push_back(cv::Point2i(x, y));
                }
            }

        int minX = 0x3f3f3f3f;
        int maxX = -0x3f3f3f3f;
        int minY = 0x3f3f3f3f;
        int maxY = -0x3f3f3f3f;
        for (int i = 0; i < (int)colorIdx.size(); ++i)
        {
            int x = colorIdx[i].x;
            int y = colorIdx[i].y;
            minX = (std::min)(minX, x);
            maxX = (std::max)(maxX, x);
            minY = (std::min)(minY, y);
            maxY = (std::max)(maxY, y);
        }


        offset = cv::Mat::eye(3, 3, CV_32FC1);
        offset.at<float>(0, 2) = -minX;
        offset.at<float>(1, 2) = -minY;

        cv::Mat srcTransed;
        cv::warpAffine(src, srcTransed, Tool::getFirst2RowsMat(offset), cv::Size(maxX - minX, maxY - minY));
        cv::Mat dstTransed;
        cv::warpAffine(dst, dstTransed, Tool::getFirst2RowsMat(offset * transMat), cv::Size(maxX - minX, maxY - minY));
        for (int i = 0; i < srcTransed.rows; ++i)
        {
            for (int j = 0; j < srcTransed.cols; ++j)
            {
                for (int k = 0; k < 4; ++k)
                {
                    if (dstTransed.at<cv::Vec4b>(i, j)[k] && !srcTransed.at<cv::Vec4b>(i, j)[k])
                        srcTransed.at<cv::Vec4b>(i, j)[k] = dstTransed.at<cv::Vec4b>(i, j)[k];
                }
            }
        }
        return srcTransed.clone();
    }


    static void rotateAndOffset(cv::Mat &img, cv::Mat rotateMat, cv::Mat &offset)
    {
        if (rotateMat.rows == 2)
            rotateMat = Tool::getFirst3RowsMat(rotateMat);
        std::vector<cv::Point2i> colorIdx;
        for (int i = 0; i < img.rows; ++i)
            for (int j = 0; j < img.cols; ++j)
            {
                int sum = img.at<cv::Vec4b>(i, j)[0] + img.at<cv::Vec4b>(i, j)[1] + img.at<cv::Vec4b>(i, j)[2];
                if (sum > 0 && sum < 255 * 4)
                {
                    int x = rotateMat.at<float>(0, 0) * j + rotateMat.at<float>(0, 1) * i + rotateMat.at<float>(0, 2);
                    int y = rotateMat.at<float>(1, 0) * j + rotateMat.at<float>(1, 1) * i + rotateMat.at<float>(1, 2);
                    colorIdx.push_back(cv::Point2i(x, y));
                }
            }

        int minX = 0x3f3f3f3f;
        int maxX = -0x3f3f3f3f;
        int minY = 0x3f3f3f3f;
        int maxY = -0x3f3f3f3f;
        for (int i = 0; i < (int)colorIdx.size(); ++i)
        {
            minX = (std::min)(minX, colorIdx[i].x);
            maxX = (std::max)(maxX, colorIdx[i].x);
            minY = (std::min)(minY, colorIdx[i].y);
            maxY = (std::max)(maxY, colorIdx[i].y);
        }

        offset = cv::Mat::eye(3, 3, CV_32FC1);
        offset.at<float>(0, 2) = -minX;
        offset.at<float>(1, 2) = -minY;
        cv::warpAffine(img, img, Tool::getFirst2RowsMat(offset * rotateMat), cv::Size(maxX - minX, maxY - minY));
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
