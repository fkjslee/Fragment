#include <QtDebug>
#include "storearea.h"
#include "ui_storearea.h"
#include <QLabel>
#include <QGraphicsItem>
#include <ui/fragmentui.h>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

namespace
{
    using namespace cv;
}

StoreArea::StoreArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StoreArea)
{
    ui->setupUi(this);
    scene = new storeScene();

    fragCtrl = FragmentsController::getController();
    int i = 0;
    for (FragmentUi *fragment : fragCtrl->getSortedFragments())
    {
        fragment->setPos(::sin((i * 6.28) / 10.0) * 150,
                         ::cos((i * 6.28) / 10.0) * 150);
        scene->addItem(fragment);
        i++;
    }
    ui->view->setScene(scene);
    ui->view->setWindowTitle("Desktop");
    ui->view->show();
    ui->view->setAcceptDrops(true);
}

void StoreArea::update()
{
    qDebug() << "update store area";
    for (FragmentUi *fragment : fragmentItems)
    {
        scene->removeItem(fragment);
    }
    fragmentItems.clear();

    for (FragmentUi *fragment : fragCtrl->getSortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
    }
    scene->update();
    QWidget::update();
    QApplication::processEvents();
}

void StoreArea::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event)
}

StoreArea::~StoreArea()
{
    delete ui;
    delete scene;
}
