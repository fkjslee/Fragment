#include "fragmentsscene.h"
#include <QMenu>
#include <ui/fragmentarea.h>
#include <ui/hintwindow.h>

FragmentsScene::FragmentsScene()
{

}

void FragmentsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *actSplit = menu.addAction(tr("拆分"));
    QAction *actJoint = menu.addAction(tr("拼接"));
    QAction *actRemoveFromDesktop = menu.addAction(tr("移出桌面"));
    QAction *actRefreshHint = menu.addAction(tr("AI推荐选中碎片"));
    connect(actSplit, &QAction::triggered, FragmentArea::getFragmentArea(), &FragmentArea::on_btnSplit_clicked);
    connect(actJoint, &QAction::triggered, FragmentArea::getFragmentArea(), &FragmentArea::on_btnJoint_clicked);
    connect(actRemoveFromDesktop, &QAction::triggered, FragmentsController::getController(), &FragmentsController::unSelectFragment);
    connect(actRefreshHint, &QAction::triggered, HintWindow::getHintWindow(), &HintWindow::actSuggestTrigged);
    menu.exec(event->screenPos());
}
