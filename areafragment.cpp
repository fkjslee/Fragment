#include "areafragment.h"
#include <Tool.h>

int AreaFragment::calcCnt = 0;
AreaFragment::AreaFragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName): FragmentUi(pieces, originalImage, fragmentName)
{
    setToolTip(fragmentName);
    initSuggestAng();
}

void AreaFragment::rotate(int ang)
{
    int suggAng = -1000;
    for (int i = 0; i < suggAngs.size(); ++i)
    {
        if (std::abs(suggAngs[i].ang - ang) < 20 && suggAngs[i].cnt != 0)
            suggAng = suggAngs[i].ang;
    }
//    if (suggAng != -1000)
//        ang = suggAng;
    this->rotateAng = ang;
    update();
}

void AreaFragment::scaledToWidth(const double scale)
{
    this->scale = scale;
    update();
}


void AreaFragment::update(const QRectF &rect)
{
    auto removeBgColorImg = originalImage.copy();
    auto mask = removeBgColorImg.createMaskFromColor(qRgb(0, 0, 0), Qt::MaskMode::MaskOutColor);
    removeBgColorImg.setAlphaChannel(mask);
    cv::Mat img = Tool::QImageToMat(removeBgColorImg);
    cv::Mat rotateMat = Tool::getRotationMatrix(img.cols / 2.0, img.rows / 2.0, Tool::angToRad(this->rotateAng));
    rotateMat = Tool::getFirst3RowsMat(rotateMat);
    Tool::rotateAndOffset(img, rotateMat, this->offset);

    QImage showImage = Tool::Mat8UC4ToQImage(img);
    showImage = showImage.scaledToWidth(int(showImage.width() * scale));
    cv::Mat fusionImg = Tool::QImageToMat(showImage);
    if (calcing)
    {
        QImage thinkingImg(":/new/pre/resources/thinking.png");
        cv::Mat thinkMat = Tool::QImageToMat(thinkingImg);
        cv::Mat smallerMat = cv::getRotationMatrix2D(cv::Point(0, 0), 0, 0.5);
        cv::warpAffine(thinkMat, thinkMat, smallerMat, cv::Size(thinkMat.rows / 2, thinkMat.cols / 2));
        int moveX = showImage.width() / 2 - thinkMat.cols / 2;
        int moveY = showImage.height();
        cv::Mat trans = cv::Mat::eye(3, 3, CV_32FC1);
        trans.at<float>(0, 2) = moveX;
        trans.at<float>(1, 2) = moveY;
        cv::Mat offset = cv::Mat::eye(3, 3, CV_32FC1);
        fusionImg = Tool::fusionImage(Tool::QImageToMat(showImage), thinkMat, trans, offset);
    }
    showImage = Tool::Mat8UC4ToQImage(fusionImg);
    setPixmap(QPixmap::fromImage(showImage));
    QGraphicsPixmapItem::update(rect);
}

void AreaFragment::initSuggestAng()
{
    cv::Mat m1_gauss;
    cv::GaussianBlur(Tool::QImageToMat(this->getOriginalImage()), m1_gauss, cv::Size(3, 3), 0);
    cv::Mat edges;
    cv::Canny(m1_gauss, edges, 50, 150, 3);
    cv::Mat lines;
    cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 50, 50, 10);
    std::vector<int> angs;
    for (int i = 0; i < lines.rows; ++i)
    {
        for (int j = 0; j < lines.cols; ++j)
        {
            cv::Vec4i line = lines.at<cv::Vec4i>(i, j);
            if (line[2] - line[0] == 0) angs.push_back(180);
            else
            {
                double rad = std::atan(1.0 * (line[3] - line[1]) / (line[2] - line[0]));
                angs.push_back(rad * 180 / CV_PI);
            }
        }
    }
    for (int i = 0; i < angs.size(); ++i)
    {
        if (angs[i] < 0) angs[i] += 360;
        bool add = false;
        for (int j = 0; j < suggAngs.size(); ++j)
        {
            if (suggAngs[j].ang == angs[i])
            {
                suggAngs[j].cnt++;
                add = true;
            }
        }
        if (!add)
        {
            SuggAng suggAng;
            suggAng.ang = angs[i];
            suggAng.cnt = 0;
            suggAngs.push_back(suggAng);
        }
    }
    std::sort(suggAngs.begin(), suggAngs.end());
}
