#include <Tool.h>
#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <fragment.h>
#include <QtDebug>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>

FragmentArea::FragmentArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FragmentArea)
{
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::SceneType::fragmentArea);
    ui->view->setScene(scene);
    //ui->autoStitch->hide();

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
    qDebug() << "update fragment area";
    for (Fragment* fragment : fragmentItems) {
        scene->removeItem(fragment);
        disconnect(fragment, &Fragment::doubleClickItem, this, &FragmentArea::sortItem);
    }
    fragmentItems.clear();

    for (Fragment* fragment : Fragment::getUnsortedFragments()) {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
        connect(fragment, &Fragment::doubleClickItem, this, &FragmentArea::sortItem);
    }
    scene->update();
    QWidget::update();
    QApplication::processEvents();
}

void FragmentArea::on_autoStitch_clicked()
{
    cv::Mat res = Tool::QImage2Mat(Fragment::getUnsortedFragments()[0]->getOriginalImage());
    qInfo() << "cmp image = " << Fragment::getUnsortedFragments()[0]->getFragmentName();
    cv::Mat originalImg = cv::imread("./p1.png");
    if (res.empty() || originalImg.empty()) {
        qWarning() << "compare empty image!";
        return;
    }
    double similarity = 1.0 * (64 - Tool::cmpWithOriginalMat(res, originalImg)) / 64 * 100;
    ui->resLabel->setText(QString("similarity: %1%").arg(similarity));
}

void FragmentArea::sortItem(Fragment *item)
{
    std::vector<JointFragment> possibleFragments = Fragment::getMostPossibleFragments(item);
    if (possibleFragments.size() == 0) {
        qWarning() << "no suitable fragment";
        return;
    }
    JointFragment possibleFragment = possibleFragments[0];
    Fragment::jointFragment(item, possibleFragment);
    update();
}

void FragmentArea::on_btnJoint_clicked()
{
    std::vector<Fragment*> jointFragments = Fragment::getSelectedFragments();
    if (jointFragments.size() == 2) {
        Fragment* f1 = jointFragments[0];
        Fragment* f2 = jointFragments[1];
        Fragment::jointFragment(f1, Fragment::mostPossibleJointMethod(f1, f2));
    }
    update();
}

void FragmentArea::on_btnSplit_clicked()
{
    Fragment::splitSelectedFragments();
    update();
}
