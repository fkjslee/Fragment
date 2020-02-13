#include <Tool.h>
#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <fragment.h>
#include <QtDebug>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <fragmentscontroller.h>

FragmentArea::FragmentArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FragmentArea)
{
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::SceneType::fragmentArea);
    ui->view->setScene(scene);
    //ui->autoStitch->hide();

    fragCtrl = FragmentsController::getController();
    int i = 0;
    for (Fragment *fragment : fragCtrl->getUnsortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        fragment->setPos(::sin((i * 6.28) / 10.0) * 150,
                         ::cos((i * 6.28) / 10.0) * 150);
        scene->addItem(fragment);
        i++;
    }
    update();
}

FragmentArea::~FragmentArea()
{
    delete ui;
    delete scene;
}

void FragmentArea::update()
{
    qDebug() << "update fragment area";
    for (Fragment *fragment : fragmentItems)
    {
        scene->removeItem(fragment);
        disconnect(fragment, &Fragment::fragmentsMoveEvents, this, &FragmentArea::fragmentsMoveEvents);
    }
    fragmentItems.clear();

    for (Fragment *fragment : fragCtrl->getUnsortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
        connect(fragment, &Fragment::fragmentsMoveEvents, this, &FragmentArea::fragmentsMoveEvents);
    }
    scene->update();
    QWidget::update();
    QApplication::processEvents();
}

void FragmentArea::fragmentsMoveEvents(QGraphicsSceneMouseEvent *event, QPoint biasPos)
{
}

void FragmentArea::on_btnJoint_clicked()
{
    std::vector<Fragment *> jointFragments = FragmentsController::getController()->getSelectedFragments();
    if (jointFragments.size() == 2)
    {
        Fragment *f1 = jointFragments[0];
        Fragment *f2 = jointFragments[1];
        fragCtrl->jointFragment(f1, FragmentsController::getController()->mostPossibleJointMethod(f1, f2));
    }
    update();
}

void FragmentArea::on_btnSplit_clicked()
{
    FragmentsController::getController()->splitSelectedFragments();
    update();
}
