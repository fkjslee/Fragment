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

FragmentUi *FragmentUi::draggingItem = nullptr;
int Piece::fragmentCnt = 0;
FragmentUi::FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName)
    : pieces(pieces), originalImage(originalImage), fragmentName(fragmentName)
{
    this->showImage = originalImage;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setPixmap(QPixmap::fromImage(showImage));
    undoPos = QPoint(0, 0);
}

void FragmentUi::scaledToWidth(const double scale)
{
    this->scale = scale;
    update();
}

void FragmentUi::update(const QRectF &rect)
{
    showImage = originalImage.scaledToWidth(int(originalImage.width() * scale));
    setPixmap(QPixmap::fromImage(showImage));
    QGraphicsPixmapItem::update(rect);
}

void FragmentUi::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    pressPos = pos();
    undoPos = pos().toPoint();
    qDebug() << "FragmentUi mousePressEvent";
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
    QGraphicsPixmapItem::mousePressEvent(event);
}

void FragmentUi::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "FragmentUi mouseReleaseEvent";
    if ((pos() - pressPos).manhattanLength() > 1e-7)
        CommonHeader::undoStack->push(new MoveUndo(this, undoPos, pos().toPoint()));
    draggingItem = nullptr;
    setCursor(Qt::OpenHandCursor);
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void FragmentUi::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
    Q_UNUSED(event)
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

void FragmentUi::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "fragment dropEvent";
    Q_UNUSED(event)
    QGraphicsPixmapItem::dropEvent(event);
}

void FragmentUi::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "fragment dragEnterEvent";
    event->accept();
    QGraphicsPixmapItem::dragEnterEvent(event);
}

void FragmentUi::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "FragmentUi dragEnterEvent";
    QGraphicsPixmapItem::dragLeaveEvent(event);
}

void FragmentUi::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "FragmentUi dragMoveEvent";
    QGraphicsPixmapItem::dragMoveEvent(event);
}

void FragmentUi::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseMoveEvent(event);
}

