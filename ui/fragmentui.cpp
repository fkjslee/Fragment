#include "fragmentui.h"
#include <QPainter>
#include <QtDebug>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QWidget>

FragmentUi *FragmentUi::draggingItem = nullptr;
FragmentUi::FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName)
    : pieces(pieces), originalImage(originalImage), fragmentName(fragmentName)
{
    this->showImage = originalImage;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setPixmap(QPixmap::fromImage(showImage));
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
    QGraphicsItem::update(rect);
}

void FragmentUi::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    qDebug() << "mousePressEvent";
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
}

void FragmentUi::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    draggingItem = nullptr;
    setCursor(Qt::OpenHandCursor);
}

void FragmentUi::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
    Q_UNUSED(event)
}

void FragmentUi::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
}

void FragmentUi::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "fragment dragEnterEvent";
    event->accept();
}

void FragmentUi::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

