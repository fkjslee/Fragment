#include <Tool.h>
#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <iostream>
#include <QLabel>
#include <QGraphicsItem>
#include <coloritem.h>
#include <QVBoxLayout>
#include <QtDebug>
#include <QGraphicsScene>

FragmentArea::FragmentArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FragmentArea)
{
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::SceneType::fragmentArea);
    ui->view->setScene(scene);
    ui->view->setWindowTitle("fragment area");
    ui->view->show();

    int i = 0;
    for (Fragment* fragment : Fragment::getUnsortedFragments()) {
        fragmentItems.emplace_back(fragment);
        fragment->setPos(::sin((i * 6.28) / 10.0) * 150,
                     ::cos((i * 6.28) / 10.0) * 150);
        scene->addItem(fragment);
        i++;
        connect(fragment, &Fragment::doubleClickItem, this, &FragmentArea::sortItem);
    }
}

FragmentArea::~FragmentArea()
{
    delete ui;
    delete scene;
}

void FragmentArea::update()
{
    for (Fragment* fragment : fragmentItems) {
        scene->removeItem(fragment);
        disconnect(fragment, &Fragment::doubleClickItem, this, &FragmentArea::sortItem);
    }
    fragmentItems.clear();

    int i = 0;
    for (Fragment* fragment : Fragment::getUnsortedFragments()) {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
        i++;
        connect(fragment, &Fragment::doubleClickItem, this, &FragmentArea::sortItem);
    }
}

void FragmentArea::on_autoStitch_clicked()
{
    update();
}

void FragmentArea::sortItem(Fragment *item)
{
    std::vector<JointFragment> possibleFragments = Fragment::getMostPossibleColorItems(item);
    if (possibleFragments.size() == 0) {
        qWarning() << "no suitable fragment";
        return;
    }
    JointFragment possibleFragment = possibleFragments[0];

    Fragment::jointFragment(item, possibleFragment);

    update();

}
