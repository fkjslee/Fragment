#include "eventgraphicsscene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QtDebug>
#include <coloritem.h>
#include <QRect>
#include <fragment.h>

void EventGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)  {
    event->acceptProposedAction();
    std::cout << "scene dragEnterEvent" << std::endl;
}

void EventGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) {
    ColorItem* draggingItem = ColorItem::getDraggingItem();
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
    qDebug() << "drag invalidOperation";
    return;
}

void EventGraphicsScene::moveFragmentFromHintToDesktop(QGraphicsSceneDragDropEvent* event)
{
    qDebug() << "drag moveFragmentFromHintToDesktop";
    ColorItem* draggingItem = ColorItem::getDraggingItem();
    Fragment::sortFragment(draggingItem->getFragment());
    draggingItem->setPos(event->scenePos());
    this->removeItem(draggingItem);
    this->addItem(draggingItem);
    emit removeFragment(draggingItem->getFragment());
}

void EventGraphicsScene::moveBetweenTwoNormalSceen(QGraphicsSceneDragDropEvent* event)
{
    qDebug() << "drag moveBetweenTwoNormalSceen";
    ColorItem* draggingItem = ColorItem::getDraggingItem();
    draggingItem->setPos(event->scenePos());
    this->removeItem(draggingItem);
    this->addItem(draggingItem);
}
