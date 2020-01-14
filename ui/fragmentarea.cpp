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

void FragmentArea::on_autoStitch_clicked()
{
    cv::Mat res = Tool::QImageToMat(fragCtrl->getUnsortedFragments()[0]->getOriginalImage());
    qInfo() << "cmp image = " << fragCtrl->getUnsortedFragments()[0]->getFragmentName();
    cv::Mat originalImg = cv::imread("./p1.png");
    if (res.empty() || originalImg.empty())
    {
        qWarning() << "compare empty image!";
        return;
    }
    double similarity = 1.0 * (64 - Tool::cmpWithOriginalMat(res, originalImg)) / 64 * 100;
    ui->resLabel->setText(QString("similarity: %1%").arg(similarity));
}

void FragmentArea::sortItem(Fragment *item)
{
    update();
}

void FragmentArea::fragmentsMoveEvents(QGraphicsSceneMouseEvent *event, QPoint biasPos)
{
//    QDrag *drag = new QDrag(event->widget());
//    QMimeData *mime = new QMimeData;
//    drag->setMimeData(mime);
//    QPixmap pixmap(int(this->width()), int(this->height()));
//    pixmap.fill(Qt::white);

//    QPainter painter(&pixmap);
//    painter.setRenderHint(QPainter::Antialiasing);
//    for (Fragment* moveFragment : Fragment::getSelectedFragments()) {
//        QImage showImage = moveFragment->getShowImage();
//        painter.drawImage(moveFragment->pos(), showImage);
//    }
//    painter.end();

////    pixmap.setMask(pixmap.createHeuristicMask());

//    drag->setPixmap(pixmap);
//    drag->setHotSpot(biasPos);

//    drag->exec();
//    setCursor(Qt::OpenHandCursor);
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
