#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <iostream>
#include <QLabel>
#include <QGraphicsItem>
#include <coloritem.h>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <fragment.h>

FragmentArea::FragmentArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FragmentArea)
{
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::SceneType::fragmentArea);
    ui->view->setScene(scene);
    ui->view->setWindowTitle("fragment area");
    ui->view->show();
    update();
}

FragmentArea::~FragmentArea()
{
    delete ui;
    delete scene;
}

void FragmentArea::update()
{
    for (ColorItem* colorItem : colorItems) {
        scene->removeItem(colorItem);
        delete colorItem;
    }
    colorItems.clear();

    int i = 0;
    for (Fragment* fragment : Fragment::getUnsortedFragments()) {
        ColorItem* item = new ColorItem(fragment);
        colorItems.emplace_back(item);
        item->setPos(::sin((i * 6.28) / 10.0) * 150,
                     ::cos((i * 6.28) / 10.0) * 150);
        scene->addItem(item);
        i++;
    }
}
