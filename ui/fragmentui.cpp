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
    auto removeBgColorImg = originalImage.copy();
    auto mask = removeBgColorImg.createMaskFromColor(qRgb(0, 0, 0), Qt::MaskMode::MaskOutColor);
    removeBgColorImg.setAlphaChannel(mask);
    cv::Mat img = Tool::QImageToMat(removeBgColorImg);
    cv::Mat rotateMat = Tool::getRotationMatrix(img.cols/2.0, img.rows/2.0, Tool::angToRad(this->rotateAng));
    rotateMat = Tool::getFirst3RowsMat(rotateMat);
    Tool::rotateAndOffset(img, rotateMat, this->offset);

    showImage = Tool::Mat8UC4ToQImage(img);
    showImage = this->showImage.scaledToWidth(int(showImage.width() * scale));
    cv::Mat fusionImg = Tool::QImageToMat(showImage);
    if (calcing) {
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

