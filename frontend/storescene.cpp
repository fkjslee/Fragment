#include "storescene.h"
#include <QMenu>
#include <fragmentscontroller.h>

storeScene::storeScene()
{

}

void storeScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *actMoveToDesktop = menu.addAction(tr("移入桌面"));
    connect(actMoveToDesktop, &QAction::triggered, FragmentsController::getController(), &FragmentsController::selectFragment);
    menu.exec(event->screenPos());
}
