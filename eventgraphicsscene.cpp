#include "eventgraphicsscene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QtDebug>
#include <coloritem.h>
#include <QRect>

void EventGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)  {
    event->acceptProposedAction();
    std::cout << "scene dragEnterEvent" << std::endl;
}

void EventGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) {
    Fragment* draggingItem = Fragment::getDraggingItem();
    if (draggingItem == nullptr)
        return;
    EventGraphicsScene* beforeScene = reinterpret_cast<EventGraphicsScene*>(draggingItem->scene());
    if (beforeScene->sceneType == SceneType::hintArea && (sceneType == SceneType::desktop)) {
        moveFragmentFromHintToDesktop(event);
    } else if ((beforeScene->sceneType == SceneType::fragmentArea) && (sceneType == SceneType::hintArea)) {
        invalidOperation(event);
    } else if ((beforeScene->sceneType == SceneType::hintArea) && (sceneType == SceneType::fragmentArea)) {
        invalidOperation(event);
    } else if ((beforeScene->sceneType == SceneType::desktop) && (sceneType == SceneType::hintArea)) {
        invalidOperation(event);
    } else {
        moveBetweenTwoNormalSceen(event);
    }
    update();
}

void EventGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
    std::cout << "EventGraphicsScene scene dropEventffffffffffffffffffff" << std::endl;
}

void EventGraphicsScene::invalidOperation(QGraphicsSceneDragDropEvent* event)
{
    Q_UNUSED(event)
    qWarning() << "drag invalidOperation";
    return;
}

void EventGraphicsScene::moveFragmentFromHintToDesktop(QGraphicsSceneDragDropEvent* event)
{
    qInfo() << "drag moveFragmentFromHintToDesktop";
    Fragment* draggingItem = Fragment::getDraggingItem();
    Fragment::sortFragment(draggingItem->getFragment());
    draggingItem->setPos(event->scenePos());
    this->removeItem(draggingItem);
    this->addItem(draggingItem);
    emit removeFragment(draggingItem->getFragment());
}

void EventGraphicsScene::moveBetweenTwoNormalSceen(QGraphicsSceneDragDropEvent* event)
{
    qInfo() << "drag moveBetweenTwoNormalSceen";
    Fragment* draggingItem = Fragment::getDraggingItem();
    draggingItem->setPos(event->scenePos());
    this->removeItem(draggingItem);
    this->addItem(draggingItem);
}
