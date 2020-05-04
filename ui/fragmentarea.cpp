#include <Tool.h>
#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <ui/fragmentui.h>
#include <QtDebug>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <fragmentscontroller.h>
#include <QMessageBox>
#include <network.h>

namespace {
int getPieceID(std::vector<Piece> pieces, QString name) {
    for (int i = 0; i < (int)pieces.size(); ++i)
        if (pieces[i].pieceName == name)
            return i;
    return -1;
}
}

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
    for (FragmentUi *fragment : fragmentItems)
    {
        scene->removeItem(fragment);
    }
    fragmentItems.clear();

    for (FragmentUi *fragment : fragCtrl->getUnsortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
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
        auto p = f1;
        f1 = f2;
        f2 = p;
        QString res = Network::sendMsg("b " + f1->getFragmentName() + " " + f2->getFragmentName());
        cv::Mat transMat = Tool::str2TransMat(res);
        fragCtrl->jointFragment(f1, 0, f2, 0, transMat);
    }
    update();
}

void FragmentArea::on_btnSplit_clicked()
{
    FragmentsController::getController()->splitSelectedFragments();
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

void FragmentArea::on_sldRotate_valueChanged(int value)
{
    FragmentsController* ctrller = FragmentsController::getController();
    for (FragmentUi* f : ctrller->getUnsortedFragments()) {
        if (f->isSelected()) {
            qInfo() << "value = " << value;
            f->rotate(value);
        }
    }
    update();
}
