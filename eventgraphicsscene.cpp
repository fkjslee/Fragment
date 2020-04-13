#include "eventgraphicsscene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QtDebug>
#include <ui/fragmentui.h>
#include <QRect>

void EventGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->acceptProposedAction();
    qDebug() << "scene dragEnterEvent";
    QGraphicsScene::dragEnterEvent(event);
}

void EventGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{

}

void EventGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "scene dropEvent";
    Q_UNUSED(event)
    QGraphicsScene::dropEvent(event);
}

void EventGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "scene mouseReleaseEvent";
    QGraphicsScene::mouseReleaseEvent(event);
}

void EventGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "scene mousePressEvent";
    QGraphicsScene::mousePressEvent(event);
}

void EventGraphicsScene::invalidOperation(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
    qWarning() << "scene drag invalidOperation";
    return;
}

void EventGraphicsScene::moveFragmentFromHintToDesktop(QGraphicsSceneDragDropEvent *event)
{
    qInfo() << "scene drag moveFragmentFromHintToDesktop";
}

void EventGraphicsScene::moveBetweenTwoNormalSceen(QGraphicsSceneDragDropEvent *event)
{
    qInfo() << "scene drag moveBetweenTwoNormalSceen";
}
