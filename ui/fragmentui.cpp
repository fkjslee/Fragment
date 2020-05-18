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

FragmentUi *FragmentUi::draggingItem = nullptr;
FragmentUi::FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName)
    : pieces(pieces), originalImage(originalImage), fragmentName(fragmentName)
{
    this->showImage = originalImage;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setPixmap(QPixmap::fromImage(showImage));
    undoPos = QPoint(0, 0);
    connect(this, &FragmentUi::refreshHintWindow, HintWindow::getHintWindow(), &HintWindow::on_refreshBtn_clicked);
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
    cv::Mat rotateMat = Tool::getRotationMatrix(img.rows/2.0, img.cols/2.0, Tool::angToRad(this->rotateAng));
    cv::warpAffine(img, img, Tool::getOpencvMat(rotateMat), img.size());
    showImage = Tool::MatToQImage(img);
    showImage = this->showImage.scaledToWidth(int(originalImage.width() * scale));
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
    if (!fragmentName.startsWith("mirror"))
        emit refreshHintWindow();
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

