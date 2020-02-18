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
    qDebug() << "scene dragLeaveEvent";
    FragmentUi *draggingItem = FragmentUi::getDraggingItem();
    if (draggingItem == nullptr)
        return;
    EventGraphicsScene *beforeScene = reinterpret_cast<EventGraphicsScene *>(draggingItem->scene());
    if (beforeScene->sceneType == SceneType::hintArea && (sceneType == SceneType::desktop))
    {
        moveFragmentFromHintToDesktop(event);
    }
    else if ((beforeScene->sceneType == SceneType::fragmentArea) && (sceneType == SceneType::hintArea))
    {
        invalidOperation(event);
    }
    else if ((beforeScene->sceneType == SceneType::hintArea) && (sceneType == SceneType::fragmentArea))
    {
        invalidOperation(event);
    }
    else if ((beforeScene->sceneType == SceneType::desktop) && (sceneType == SceneType::hintArea))
    {
        invalidOperation(event);
    }
    else
    {
        moveBetweenTwoNormalSceen(event);
    }
    update();
    QGraphicsScene::dragLeaveEvent(event);
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
    FragmentUi *draggingItem = FragmentUi::getDraggingItem();
    draggingItem->setPos(event->scenePos());
    this->removeItem(draggingItem);
    this->addItem(draggingItem);
//    emit removeFragment(draggingItem->getFragment());
}

void EventGraphicsScene::moveBetweenTwoNormalSceen(QGraphicsSceneDragDropEvent *event)
{
    qInfo() << "scene drag moveBetweenTwoNormalSceen";
    FragmentUi *draggingItem = FragmentUi::getDraggingItem();
    draggingItem->setPos(event->scenePos() - draggingItem->getBiasPos());
    this->removeItem(draggingItem);
    this->addItem(draggingItem);
}
