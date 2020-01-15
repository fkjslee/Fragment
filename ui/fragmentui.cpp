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
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

QRectF FragmentUi::boundingRect() const
{
    return showImage.rect();
}

void FragmentUi::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawImage(QPoint(0, 0), showImage);
}

void FragmentUi::update(const QRectF &rect)
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

void FragmentUi::scaledToWidth(const double scale)
{
    this->scale = scale;
    update();
}

void FragmentUi::reverseSelectState()
{
    selected = !selected;
    update();
}

void FragmentUi::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
}

void FragmentUi::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    draggingItem = nullptr;
    setCursor(Qt::OpenHandCursor);
}

void FragmentUi::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "mousePressEvent";
    selected = !selected;
    update();
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

