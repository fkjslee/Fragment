#include "coloritem.h"
#include <iostream>
#include <QTextStream>
#include <QtDebug>

ColorItem::ColorItem()
    : color(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256))
{
    setToolTip(QString("QColor(%1, %2, %3)\n%4")
              .arg(color.red()).arg(color.green()).arg(color.blue())
              .arg("Click and drag this color onto the robot!"));
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
}

QRectF ColorItem::boundingRect() const
{
    return QRectF(-15.5, -15.5, 34, 34);
}

void ColorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-12, -12, 30, 30);
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(color));
    painter->drawEllipse(-15, -15, 30, 30);
}

void ColorItem::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    std::cout << "color item mouse press event" << std::endl;
    setCursor(Qt::ClosedHandCursor);
}

void ColorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    std::cout << "color item mouse release event" << std::endl;
    setCursor(Qt::OpenHandCursor);
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
    emit sendColorItemDragging(event);
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    mime->setColorData(color);
//    QString transData;
//    QTextStream(&transData) << int(this);
//    mime->setText(transData);
    QPixmap pixmap(34, 34);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.translate(15, 15);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, nullptr, nullptr);
    painter.end();

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(15, 20));


    drag->exec();
    setCursor(Qt::OpenHandCursor);
}
