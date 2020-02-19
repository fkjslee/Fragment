#include <QtDebug>
#include "desktop.h"
#include "ui_desktop.h"
#include <QLabel>
#include <QGraphicsItem>
#include <ui/fragmentui.h>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

using namespace cv;

Desktop::Desktop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Desktop)
{
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::SceneType::desktop);

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

void Desktop::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event)
}

Desktop::~Desktop()
{
    delete ui;
    delete scene;
}