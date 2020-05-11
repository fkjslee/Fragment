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

FragmentArea* FragmentArea::fragmentArea = nullptr;
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
    fragmentArea = this;
    scene = new FragmentsScene;
    ui->view->setScene(scene);

    fragCtrl = FragmentsController::getController();
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
//        qInfo() << "pos = " << fragment->getFragmentName() << " " << fragment->pos();
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

void FragmentArea::updateFragmentsPos()
{
    QRect windowRect = this->rect();
    qInfo() << "rect = " << windowRect;
    int i = 0;
    int N = (int)fragCtrl->getUnsortedFragments().size();
    for (FragmentUi *fragment : fragCtrl->getUnsortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        fragment->setPos(::sin((i * 6.28) / N) * windowRect.width() / 5,
                         ::cos((i * 6.28) / N) * windowRect.height() / 5);
        scene->addItem(fragment);
        i++;
    }
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
        if (res[0] > '9' || res[0] < '0') {
            QMessageBox::warning(nullptr, QObject::tr("joint error"), res,
                                  QMessageBox::Cancel);
            return;
        }
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

void FragmentArea::on_sldRotate_valueChanged(int value)
{
    FragmentsController* ctrller = FragmentsController::getController();
    for (FragmentUi* f : ctrller->getUnsortedFragments()) {
        if (f->isSelected()) {
            f->rotate(value);
        }
    }
    update();
}
