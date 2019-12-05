#include <QtDebug>
#include "desktop.h"
#include "ui_desktop.h"
#include <iostream>
#include <QLabel>
#include <QGraphicsItem>
#include <robothead.h>
#include <coloritem.h>
#include <QVBoxLayout>
#include <eventgraphicsscene.h>
#include <eventgraphicsview.h>
#include "fragment.h"

Desktop::Desktop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Desktop)
{
    ui->setupUi(this);
    EventGraphicsScene *scene = new EventGraphicsScene(EventGraphicsScene::SceneType::desktop);

    Fragment::createFragments();
    int i = 0;
    for (Fragment* fragment : Fragment::getSortedFragments()) {
        ColorItem* item = fragment->getItemShape();
        item->setPos(::sin((i * 6.28) / 10.0) * 150,
                     ::cos((i * 6.28) / 10.0) * 150);
        scene->addItem(item);
        i++;
    }
    RobotHead *robothead = new RobotHead;
    robothead->setPos(0, -20);
    scene->addItem(robothead);
    ui->view->setScene(scene);
    ui->view->setWindowTitle("Desktop");
    ui->view->show();
}

void Desktop::dropEvent(QDropEvent *event)
{
}

Desktop::~Desktop()
{
    delete ui;
}
