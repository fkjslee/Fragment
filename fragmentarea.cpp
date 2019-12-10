#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <iostream>
#include <QLabel>
#include <QGraphicsItem>
#include <coloritem.h>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <eventgraphicsscene.h>
#include <fragment.h>

FragmentArea::FragmentArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FragmentArea)
{
    ui->setupUi(this);
    EventGraphicsScene *scene = new EventGraphicsScene(EventGraphicsScene::SceneType::fragmentArea);
    int i = 0;
    for (Fragment* fragment : Fragment::getUnsortedFragments()) {
        ColorItem* item = new ColorItem(fragment);
        item->setPos(::sin((i * 6.28) / 10.0) * 150,
                     ::cos((i * 6.28) / 10.0) * 150);
        scene->addItem(item);
        i++;
    }
    ui->view->setScene(scene);
    ui->view->setWindowTitle("fragment area");
    ui->view->show();
}

FragmentArea::~FragmentArea()
{
    delete ui;
}
