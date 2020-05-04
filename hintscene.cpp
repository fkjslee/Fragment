#include "hintscene.h"
#include <QMenu>
#include <ui/hintwindow.h>

HintScene::HintScene()
{

}

void HintScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *actMoveToDesktop = menu.addAction(tr("自动拼接"));
    connect(actMoveToDesktop, &QAction::triggered, HintWindow::getHintWindow(), &HintWindow::on_btnAutoJoint_clicked);
    menu.exec(event->screenPos());
}

