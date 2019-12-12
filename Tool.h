#ifndef TOOL_H
#define TOOL_H

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QtDebug>

using namespace cv;

class Tool {
public:
    static cv::Mat QImage2Mat(QImage image)
    {
        cv::Mat mat;
        switch(image.format())
        {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
    //        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
    //        cv::cvtColor(mat, mat, COLOR_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        default: ;
        }
        return mat;
    }

    static QImage MatToQImage(const cv::Mat& mat)
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
                memcpy(pDest, pSrc, mat.cols);
                pSrc += mat.step;
            }
            return image;
        }
        // 8-bits unsigned, NO. OF CHANNELS = 3
        else if(mat.type() == CV_8UC3)
        {
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        else if(mat.type() == CV_8UC4)
        {
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
            return image.copy();
        }
        else
        {
            qCritical() << "ERROR: Mat could not be converted to QImage.";
            return QImage();
        }
    }

    static double calcLeftRightAbsGrayscale(const cv::Mat& left, const cv::Mat& rhs) {
        double res = 0;
        if (left.type() != rhs.type() || left.rows != rhs.rows || left.type() != CV_8UC4)
            return 0x3f3f3f3f;
        int c = left.cols - 1;
        for (int r = 0; r < left.rows; ++r) {
            for (int channel = 0; channel < 4; ++channel)
                res += abs(left.at<Vec4b>(r, c)[channel] - rhs.at<Vec4b>(r, 0)[channel]);
        }
        return res / left.rows;
    }

    static double calcUpDownAbsGrayscale(const cv::Mat& up, const cv::Mat& down) {
        double res = 0;
        if (up.type() != down.type() || up.cols != down.cols || up.type() != CV_8UC4)
            return 0x3f3f3f3f;
        int r = up.rows - 1;
        for (int c = 0; c < up.cols; ++c) {
            for (int channel = 0; channel < 4; ++channel)
                res += abs(up.at<Vec4b>(r, c)[channel] - down.at<Vec4b>(0, c)[channel]);
        }
        return res / up.cols;
    }

};

#endif // TOOL_H