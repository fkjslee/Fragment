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
#include <ui/hintwindow.h>

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

void FragmentUi::rotate(double ang)
{
    this->rotateAng = ang;
    update();
}

void FragmentUi::update(const QRectF &rect)
{
    QMatrix matrix;
    matrix.rotate(this->rotateAng);
    this->showImage = this->originalImage.transformed(matrix,Qt::FastTransformation);
    showImage = this->showImage.scaledToWidth(int(originalImage.width() * scale));
    setPixmap(QPixmap::fromImage(showImage));
    QGraphicsPixmapItem::update(rect);
}

void FragmentUi::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
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
//    if (change == ItemPositionChange && scene()) {
//        // value is the new position.

//        QPointF newPos = value.toPointF();
//        QRectF rect = scene()->sceneRect();
//        if (!rect.contains(newPos)) {
//            // Keep the item inside the scene rect.
//            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
//            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
//            return newPos;
//        }
//    }
    return QGraphicsItem::itemChange(change, value);
}

