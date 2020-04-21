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
#include <NumCpp.hpp>

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
    return;
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
#ifdef MINE
        fragCtrl->jointFragment(f1, FragmentsController::getController()->mostPossibleJointMethod(f1, f2));
#else
//        nc::NdArray<double> t(3, 3);
//        fragCtrl->jointFragment(f1, f2, t);
#endif
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

void FragmentArea::on_btnAutoJoint_clicked()
{
    auto selectFragments = fragCtrl->getSelectedFragments();
    if (selectFragments.size() != 1) {
        QMessageBox::critical(nullptr, QObject::tr("auto joint error"), QObject::tr("please choose one fragments to auto joint"),
                              QMessageBox::Cancel);
        return;
    }

    for (const QString &fragName : selectFragments[0]->getFragmentName().split(" ")) {
        qDebug() << "selectFragment name = " << selectFragments[0]->getFragmentName();
        QString res = Network::sendMsg("a " + fragName);
        res.replace("[", "");
        res.replace("]", "");
        res.replace("\n", "");
        QStringList msgList = res.split(" ");
        QStringList msgList2;
        for (QString s : msgList)
            if (s != "")
                msgList2.append(s);
        bool jointSuccess = false;
        for (int i = 0; i < msgList2.length(); i += 10) {
            // if two fragments in one peace, pass them
            if (selectFragments[0]->getFragmentName().split(" ").contains(msgList2[i])) {
                continue;
            }
            qDebug() << "another name = " << msgList2[i];
            FragmentUi* anotherFragment = fragCtrl->findFragmentByName(msgList2[i]);
            if (anotherFragment == nullptr) {
                qCritical() << "another fragment is null";
                return;
            }
            QStringList transform;
            for (int j = 0; j < 9; ++j)
                transform.append(msgList2[i+1+j]);
            fragCtrl->jointFragment(selectFragments[0], anotherFragment, transform);
            jointSuccess = true;
            break;
        }
        if (jointSuccess) break;
    }
}
