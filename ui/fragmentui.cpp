#include "fragmentui.h"
#include <QPainter>
#include <QtDebug>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QWidget>
#include <ui/mainwindow.h>
#include <commands.h>
#include <Tool.h>
#include <CommonHeader.h>
#include <opencv2/opencv.hpp>
#include <ui/hintwindow.h>
#include <qrgb.h>

namespace {
void rotateAndOffset(cv::Mat& img, const cv::Mat& rotateMat, cv::Mat& offset) {
    std::vector<cv::Point2i> colorIdx;
    for (int i = 0; i < img.rows; ++i)
        for (int j = 0; j < img.cols; ++j)
            if ((img.at<cv::Vec4b>(i, j)[0]) || img.at<cv::Vec4b>(i, j)[1] || img.at<cv::Vec4b>(i, j)[2] || img.at<cv::Vec4b>(i, j)[3])
                colorIdx.push_back(cv::Point2i(j, i));

    int minX = 0x3f3f3f3f;
    int maxX = -0x3f3f3f3f;
    int minY = 0x3f3f3f3f;
    int maxY = -0x3f3f3f3f;
    cv::Mat opencvRotateMat = rotateMat.clone();
    for (int i = 0; i < (int)colorIdx.size(); ++i) {
        int x = opencvRotateMat.at<float>(0, 0) * colorIdx[i].x + opencvRotateMat.at<float>(0, 1) * colorIdx[i].y + opencvRotateMat.at<float>(0, 2);
        int y = opencvRotateMat.at<float>(1, 0) * colorIdx[i].x + opencvRotateMat.at<float>(1, 1) * colorIdx[i].y + opencvRotateMat.at<float>(1, 2);
        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    offset = cv::Mat::eye(3, 3, CV_32FC1);
    offset.at<float>(0, 2) = -minX;
    offset.at<float>(1, 2) = -minY;
    cv::warpAffine(img, img, Tool::getFirst2RowsMat(offset * rotateMat), cv::Size(maxX - minX, maxY - minY));
}
}

FragmentUi *FragmentUi::draggingItem = nullptr;
int FragmentUi::calcCnt = 0;
FragmentUi::FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName,  Platfrom platform)
    : pieces(pieces), originalImage(originalImage), fragmentName(fragmentName)
{
    this->platform = platform;
    this->showImage = originalImage;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setPixmap(QPixmap::fromImage(showImage));
    undoPos = QPoint(0, 0);
    connect(this, &FragmentUi::refreshHintWindow, HintWindow::getHintWindow(), &HintWindow::actSuggestTrigged);
}

void FragmentUi::scaledToWidth(const double scale)
{
    this->scale = scale;
    update();
}

void FragmentUi::rotate(int ang)
{
    this->rotateAng = ang;
    update();
}

void FragmentUi::update(const QRectF &rect)
{
    cv::Mat img = Tool::QImageToMat(this->originalImage).clone();
    cv::Mat rotateMat = Tool::getRotationMatrix(img.cols/2.0, img.rows/2.0, Tool::angToRad(this->rotateAng));
    rotateMat = Tool::getFirst3RowsMat(rotateMat);
    rotateAndOffset(img, rotateMat, this->offset);

    showImage = Tool::MatToQImage(img);
    showImage = this->showImage.scaledToWidth(int(showImage.width() * scale));
    cv::Mat fusionImg = Tool::QImageToMat(showImage);
    if (calcing) {
        qInfo() << "here 83";
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
    showImage = Tool::MatToQImage(fusionImg);
    auto mask = showImage.createMaskFromColor(qRgb(0, 0, 0), Qt::MaskMode::MaskOutColor);
    showImage.setAlphaChannel(mask);
    setPixmap(QPixmap::fromImage(showImage));
    QGraphicsPixmapItem::update(rect);
}

void FragmentUi::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    FragmentArea::getFragmentArea()->setRotateAng(rotateAng);
    pressPos = pos();
    undoPos = pos().toPoint();
    setCursor(Qt::ClosedHandCursor);
    QGraphicsPixmapItem::mousePressEvent(event);
}

void FragmentUi::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ((pos() - pressPos).manhattanLength() > 1e-7)
        CommonHeader::undoStack->push(new MoveUndo(this, undoPos, pos().toPoint()));
    setCursor(Qt::OpenHandCursor);
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

QVariant FragmentUi::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

