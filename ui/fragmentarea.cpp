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
    qInfo() << "update fragment area";
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

void FragmentArea::updateFragmentsPos()
{
    QRect windowRect = this->rect();
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

void FragmentArea::setRotateAng(int value)
{
    ui->sldRotate->setValue(value);
}

void FragmentArea::on_btnJoint_clicked()
{
    std::vector<FragmentUi *> jointFragments = FragmentsController::getController()->getSelectedFragments();
    if (jointCheck() == false) return;
    FragmentUi *f1 = jointFragments[0];
    FragmentUi *f2 = jointFragments[1];

    auto p1s = f1->getPieces();
    auto p2s = f2->getPieces();

    bool jointSucc = false;
    for (int i = 0; i < (int)p1s.size(); ++i) {
        for (int j = 0;  j< (int)p2s.size(); ++j) {
            Piece p1 = p1s[i];
            Piece p2 = p2s[j];
            QString res = Network::sendMsg("b " + p1.pieceName + " " + p2.pieceName);
            cv::Mat transMat = Tool::str2TransMat(res);
            if (transMat.rows != 0) {
                fragCtrl->jointFragment(f1, i, f2, j, transMat);
                jointSucc = true;
                break;
            }
        }
        if (jointSucc) break;
    }
    if (!jointSucc) {
        QMessageBox::warning(nullptr, QObject::tr("joint error"), "These two fragments are not aligned.",
                              QMessageBox::Cancel);
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

void FragmentArea::on_btnJointForce_clicked()
{
    if (jointCheck() == false) return;
    std::vector<FragmentUi *> jointFragments = FragmentsController::getController()->getSelectedFragments();
    FragmentUi *f1 = jointFragments[0];
    FragmentUi *f2 = jointFragments[1];

    cv::Mat img1 = Tool::QImageToMat(f1->getOriginalImage()).clone();
    cv::Mat rotateMat1 = Tool::getRotationMatrix(img1.rows/2.0, img1.cols/2.0, Tool::angToRad(f1->rotateAng));
    cv::Mat rot1Inv = Tool::getFirst3RowsMat(rotateMat1).clone();
    cv::invert(rot1Inv, rot1Inv);

    cv::Mat img2 = Tool::QImageToMat(f2->getOriginalImage()).clone();
    cv::Mat rotateMat2 = Tool::getRotationMatrix(img2.rows/2.0, img2.cols/2.0, Tool::angToRad(f2->rotateAng));

    QPointF movePos = f2->scenePos() - f1->scenePos();
    cv::Mat transMat = cv::Mat::eye(2, 3, CV_32FC1);
    transMat.at<float>(0, 2) = movePos.y() * (100.0 / MainWindow::mainWindow->getZoomSize());
    transMat.at<float>(1, 2) = movePos.x() * (100.0 / MainWindow::mainWindow->getZoomSize());
    cv::Mat trans1Inv = f1->getPieces()[0].transMat.clone();
    cv::invert(trans1Inv, trans1Inv);
    transMat = rot1Inv * trans1Inv * Tool::getFirst3RowsMat(transMat) * Tool::getFirst3RowsMat(rotateMat2) * f2->getPieces()[0].transMat;
    fragCtrl->jointFragment(f1, 0, f2, 0, transMat);
}

bool FragmentArea::jointCheck()
{
    std::vector<FragmentUi *> jointFragments = FragmentsController::getController()->getSelectedFragments();
    if (jointFragments.size() != 2)
    {
        QMessageBox::critical(nullptr, QObject::tr("joint error"), QObject::tr("please choose two fragments to joint"),
                              QMessageBox::Cancel);
        return false;
    }
    for (FragmentUi *f : FragmentsController::getController()->getSortedFragments())
    {
        if (f == jointFragments[0] || f == jointFragments[1])
        {
            QMessageBox::critical(nullptr, QObject::tr("joint error"), QObject::tr("fragments only can be joint in joint area"),
                                  QMessageBox::Cancel);
            return false;
        }
    }
    return true;
}
