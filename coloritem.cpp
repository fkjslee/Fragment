#include "coloritem.h"
#include <iostream>
#include <QTextStream>
#include <QtDebug>

ColorItem* ColorItem::draggingItem = nullptr;
ColorItem::ColorItem(Fragment* fragment, const QString& picPath) :
    color(fragment->getProperty()), fragment(fragment), picPath(picPath)
{
    setToolTip(fragment->getFragmentName());
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
}

QRectF ColorItem::boundingRect() const
{
    return fragment->getImage().rect();
}

void ColorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawImage(QPoint(0, 0), fragment->getImage());
}

void ColorItem::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    qDebug() << "color item mouse press event";
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
}

void ColorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    std::cout << "color item mouse release event" << std::endl;
    draggingItem = nullptr;
    setCursor(Qt::OpenHandCursor);
}

void ColorItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClickItem(this);
    Q_UNUSED(event)
}

void ColorItem::dropEvent(QDropEvent *e)
{
    Q_UNUSED(e)
    std::cout << "xxx drop event" << std::endl;
}

void ColorItem::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept();
}

void ColorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        return;
    }
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    mime->setColorData(color);
    QPixmap pixmap(fragment->getImage().width(), fragment->getImage().height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
//    painter.translate(15, 15);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, nullptr, nullptr);
    painter.end();

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(15, 20));


    drag->exec();
    setCursor(Qt::OpenHandCursor);
}
