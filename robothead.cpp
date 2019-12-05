#include "robothead.h"
#include <iostream>

RobotHead::RobotHead(QGraphicsItem *parent)
    : RobotPart(parent)
{
    std::cout << " robot head constructed" << std::endl;
}

QRectF RobotHead::boundingRect() const
{
    return QRectF(-15, -50, 30, 50);
}

void RobotHead::paint(QPainter *painter,
             const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)
  if (pixmap.isNull()) {
      painter->setBrush(dragOver ? color.lighter(130) : color);
      painter->drawRoundedRect(-10, -30, 20, 30, 25, 25, Qt::RelativeSize);
      painter->setBrush(Qt::white);
      painter->drawEllipse(-7, -3 - 20, 7, 7);
      painter->drawEllipse(0, -3 - 20, 7, 7);
      painter->setBrush(Qt::black);
      painter->drawEllipse(-5, -1 - 20, 2, 2);
      painter->drawEllipse(2, -1 - 20, 2, 2);
      painter->setPen(QPen(Qt::black, 2));
      painter->setBrush(Qt::NoBrush);
      painter->drawArc(-6, -2 - 20, 12, 15, 190 * 16, 160 * 16);
  } else {
      painter->scale(.2272, .2824);
      painter->drawPixmap(QPointF(-15 * 4.4, -50 * 3.54), pixmap);
  }
}

void RobotHead::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    std::cout << "drag enter event" << std::endl;
//    if (event->mimeData()->hasImage()) {
//        event->setAccepted(true);
//        dragOver = true;
//        update();
//    } else {
//        RobotPart::dragEnterEvent(event);
//    }
}

void RobotHead::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    std::cout << "drop event" << std::endl;
//    if (event->mimeData()->hasImage()) {
//        dragOver = false;
//        pixmap = qvariant_cast<QPixmap>(event->mimeData()->imageData());
//        update();
//    } else {
//        RobotPart::dropEvent(event);
//    }
}
