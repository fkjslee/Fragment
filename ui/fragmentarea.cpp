#include <Tool.h>
#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <ui/fragmentui.h>
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
    for (FragmentUi *fragment : fragCtrl->getUnsortedFragments())
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
    for (FragmentUi *fragment : fragmentItems)
    {
        scene->removeItem(fragment);
        disconnect(fragment, &FragmentUi::fragmentsMoveEvents, this, &FragmentArea::fragmentsMoveEvents);
    }
    fragmentItems.clear();

    for (FragmentUi *fragment : fragCtrl->getUnsortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
        connect(fragment, &FragmentUi::fragmentsMoveEvents, this, &FragmentArea::fragmentsMoveEvents);
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
    std::vector<FragmentUi *> jointFragments = FragmentsController::getController()->getSelectedFragments();
    if (jointFragments.size() == 2)
    {
        FragmentUi *f1 = jointFragments[0];
        FragmentUi *f2 = jointFragments[1];
        fragCtrl->jointFragment(f1, FragmentsController::getController()->mostPossibleJointMethod(f1, f2));
    }
    update();
}

void FragmentArea::on_btnSplit_clicked()
{
    FragmentsController::getController()->splitSelectedFragments();
    update();
}
