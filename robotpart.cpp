#include "robotpart.h"


RobotPart::RobotPart(QGraphicsItem *parent)
      : QGraphicsObject(parent), color(Qt::lightGray), dragOver(false)
{
    setAcceptDrops(true);
}

void RobotPart::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
      event->setAccepted(true);
      dragOver = true;
      update();
    } else {
      event->setAccepted(false);
    }
}

void RobotPart::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
    dragOver = false;
    update();
}

void RobotPart::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  dragOver = false;
  if (event->mimeData()->hasColor())
      color = qvariant_cast<QColor>(event->mimeData()->colorData());
  update();
}
