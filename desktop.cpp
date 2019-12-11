#include <QtDebug>
#include "desktop.h"
#include "ui_desktop.h"
#include <iostream>
#include <QLabel>
#include <QGraphicsItem>
#include <coloritem.h>
#include <QVBoxLayout>
#include "fragment.h"

Desktop::Desktop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Desktop)
{
    Fragment::createFragments();
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::SceneType::desktop);

    int i = 0;
    for (Fragment* fragment : Fragment::getSortedFragments()) {
        ColorItem* item = new ColorItem(fragment);
        item->setPos(::sin((i * 6.28) / 10.0) * 150,
                     ::cos((i * 6.28) / 10.0) * 150);
        scene->addItem(item);
        i++;
    }
    ui->view->setScene(scene);
    ui->view->setWindowTitle("Desktop");
    ui->view->show();
    ui->view->setAcceptDrops(true);
}

void Desktop::dropEvent(QDropEvent *event)
{
}

Desktop::~Desktop()
{
    delete ui;
    delete scene;
}
