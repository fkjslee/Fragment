#include "fragment.h"
#include <QTextStream>
#include <QtDebug>
#include <opencv2/opencv.hpp>
#include <QDir>
#include <Tool.h>
#include <algorithm>
#include <QStyleOptionGraphicsItem>
#include <QMetaEnum>
#include <Tool.h>

using namespace cv;

Fragment *Fragment::draggingItem = nullptr;
Fragment::Fragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName)
    : pieces(pieces), originalImage(originalImage), fragmentName(fragmentName)
{
    this->showImage = originalImage;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

void Fragment::imageSizeChanged(const int value)
{
    qDebug() << "change value = " << value;
}

QRectF Fragment::boundingRect() const
{
    return showImage.rect();
}

void Fragment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawImage(QPoint(0, 0), showImage);
}

void Fragment::scaledToWidth(const double scale)
{
    this->scale = scale;
    update();
}

void Fragment::reverseSelectState()
{
    selected = !selected;
    update();
}

void Fragment::update(const QRectF &rect)
{
    showImage = originalImage.scaledToWidth(int(originalImage.width() * scale));
    int alpha;
    if (selected) alpha = 100;
    else alpha = 255;
    QPainter painter(&showImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(0, 0, QPixmap::fromImage(showImage));
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.fillRect(showImage.rect(), QColor(0, 0, 0, alpha));
    painter.end();
    QGraphicsItem::update(rect);
}

void Fragment::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
}

void Fragment::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    draggingItem = nullptr;
    setCursor(Qt::OpenHandCursor);
}

void Fragment::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mousePressEvent";
    selected = !selected;
    update();
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
    Q_UNUSED(event)
}

void Fragment::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
}

void Fragment::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "fragment dragEnterEvent";
    event->accept();
}

void Fragment::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
            .length() < QApplication::startDragDistance())
    {
        return;
    }
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    QPixmap pixmap(showImage.width(), showImage.height());
    pixmap.fill(Qt::white);
    qDebug() << "showImage.size = " << showImage.size();

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    paint(&painter, nullptr, nullptr);
    painter.end();

//    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    biasPos = event->scenePos() - this->scenePos();
    drag->setHotSpot(biasPos.toPoint());

    drag->exec();
    setCursor(Qt::OpenHandCursor);
}
