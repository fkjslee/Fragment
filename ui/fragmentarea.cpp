#include <Tool.h>
#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <ui/fragmentui.h>
#include <QtDebug>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <fragmentscontroller.h>
#include <QMessageBox>

FragmentArea::FragmentArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FragmentArea)
{
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::SceneType::fragmentArea);
    ui->view->setScene(scene);
//    ui->autoStitch->hide();

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
    QImage img(100, 200, QImage::Format_RGB32);

//    ui->view->setBackgroundBrush(img);
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
//    SceneBackground background;
//    background.setScale(int(scene->width()), int(scene->height()));
//    qDebug() << background.width();
//    background.save("C:\\Users\\fkjslee\\Desktop\\ant_1.3.4\\img.jpg");
//    ui->view->setBackgroundBrush(background);
    for (FragmentUi *fragment : fragmentItems)
    {
        scene->removeItem(fragment);
//        disconnect(fragment, &FragmentUi::fragmentsMoveEvents, this, &FragmentArea::fragmentsMoveEvents);
    }
    fragmentItems.clear();

    for (FragmentUi *fragment : fragCtrl->getUnsortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
//        connect(fragment, &FragmentUi::fragmentsMoveEvents, this, &FragmentArea::fragmentsMoveEvents);
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
    if (jointFragments.size() != 2)
    {
        QMessageBox::critical(nullptr, QObject::tr("joint error"), QObject::tr("please choose two fragments to joint"),
                              QMessageBox::Cancel);
        return;
    }
    for (FragmentUi *f : FragmentsController::getController()->getSortedFragments())
    {
        if (f == jointFragments[0] || f == jointFragments[1])
        {
            QMessageBox::critical(nullptr, QObject::tr("joint error"), QObject::tr("fragments only can be joint in joint area"),
                                  QMessageBox::Cancel);
            return;
        }
    }
    if (jointFragments.size() == 2)
    {
        FragmentUi *f1 = jointFragments[0];
        FragmentUi *f2 = jointFragments[1];
        fragCtrl->jointFragment(f1, FragmentsController::getController()->mostPossibleJointMethod(f1, f2), this);
    }
    update();
}

void FragmentArea::on_btnSplit_clicked()
{
    FragmentsController::getController()->splitSelectedFragments(this);
    update();
}

void FragmentArea::on_autoStitch_clicked()
{
    update();
}

void FragmentArea::on_unSelect_clicked()
{
    FragmentsController::getController()->unSelectFragment();
}
