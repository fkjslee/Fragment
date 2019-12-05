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
    std::cout << "scene dragLeaveEvent" << std::endl;
    Fragment* draggingFragment = Fragment::getDraggingItem();
    draggingFragment->getItemShape()->setPos(event->scenePos());
    if(sceneType == SceneType::desktop) {
        Fragment::sortFragment(draggingFragment);
    } else if (sceneType == SceneType::fragmentArea) {
        Fragment::unsortFragment(draggingFragment);
    }
    qDebug() << "sorted fragments size = " << Fragment::getSortedFragments().size();
    qDebug() << "unsorted fragments size = " << Fragment::getUnsortedFragments().size();
    this->removeItem(draggingFragment->getItemShape());
    this->addItem(draggingFragment->getItemShape());
//    ColorItem * item = new ColorItem();
//    item->setPos(event->scenePos());
//    addItem(item);
    update();
}

void EventGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
    std::cout << "desktop scene dropEventffffffffffffffffffff" << std::endl;
}
