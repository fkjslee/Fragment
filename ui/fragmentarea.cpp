#include <Tool.h>
#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <QtDebug>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <fragmentscontroller.h>
#include <QMessageBox>
#include <network.h>
#include "refreshthread.h"
#include <QMutex>

FragmentArea *FragmentArea::fragmentArea = nullptr;
QMutex FragmentArea::lock;

namespace
{
    const Piece *findPieceById(const std::vector<Piece> &pieces, int pieceID)
    {
        for (const Piece &p : pieces)
        {
            if (p.pieceID == pieceID) return &p;
        }
        return nullptr;
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
    ui->btnJoint->hide();
    if (MainWindow::expMode != 2)
        ui->btnReSort->hide();
}

FragmentArea::~FragmentArea()
{
    delete ui;
    delete scene;
}

void FragmentArea::update()
{
    for (AreaFragment *fragment : fragmentItems)
    {
        scene->removeItem(fragment);
    }
    fragmentItems.clear();

    for (AreaFragment *fragment : fragCtrl->getUnsortedFragments())
    {
        fragmentItems.emplace_back(fragment);
        scene->addItem(fragment);
    }

    ui->resLabel->setText("score: " + QString::number(FragmentsController::getController()->calcScore()));
    scene->update();
    QWidget::update();
}

void FragmentArea::setRotateAng(int value, bool man)
{
    if (FragmentsController::getController()->getSelectedFragments().size() == 1)
        ui->sldRotate->setValue(value);
    manRotate = man;
}

void FragmentArea::on_btnSplit_clicked()
{
    FragmentsController::getController()->splitSelectedFragments();
    update();
}

void FragmentArea::on_sldRotate_valueChanged(int value)
{
    FragmentsController *ctrller = FragmentsController::getController();
    for (AreaFragment *f : ctrller->getUnsortedFragments())
    {
        if (f->isSelected())
        {
            f->rotate(1.0 * value / 100);
            if (manRotate)
                f->setMovedSign();
        }
    }
    manRotate = true;
    update();
}

void FragmentArea::on_btnJointForce_clicked()
{
    qInfo() << "click btn joint force";
    if (jointCheck() == false) return;
    std::vector<AreaFragment *> jointFragments = FragmentsController::getController()->getSelectedFragments();
    AreaFragment *f1 = jointFragments[0];
    AreaFragment *f2 = jointFragments[1];

    cv::Mat img1 = Tool::QImageToMat(f1->getOriginalImage()).clone();
    cv::Mat rotateMat1 = Tool::getRotationMatrix(img1.cols / 2.0, img1.rows / 2.0, Tool::angToRad(f1->rotateAng));

    cv::Mat img2 = Tool::QImageToMat(f2->getOriginalImage()).clone();
    cv::Mat rotateMat2 = Tool::getRotationMatrix(img2.cols / 2.0, img2.rows / 2.0, Tool::angToRad(f2->rotateAng));

    float moveX = (f2->scenePos().x()) - (f1->scenePos().x());
    float moveY = (f2->scenePos().y()) - (f1->scenePos().y());
    cv::Mat transMat = cv::Mat::eye(3, 3, CV_32FC1);
    transMat.at<float>(0, 2) = moveX * (100.0 / MainWindow::mainWindow->getZoomSize());
    transMat.at<float>(1, 2) = moveY * (100.0 / MainWindow::mainWindow->getZoomSize());
    cv::Mat f1Changed = f1->getOffsetMat() * Tool::getFirst3RowsMat(rotateMat1) * f1->getPieces()[0].transMat;
    cv::Mat f2Changed = f2->getOffsetMat() * Tool::getFirst3RowsMat(rotateMat2) * f2->getPieces()[0].transMat;
    transMat = f1Changed.inv() * transMat * f2Changed.clone();
    fragCtrl->jointFragment(f1, &f1->getPieces()[0], f2, &f2->getPieces()[0], transMat);
}

bool FragmentArea::jointCheck()
{
    std::vector<AreaFragment *> jointFragments = FragmentsController::getController()->getSelectedFragments();
    if (jointFragments.size() != 2)
    {
        QMessageBox::critical(nullptr, QObject::tr("joint error"), QObject::tr("please choose two fragments to joint"),
                              QMessageBox::Cancel);
        return false;
    }
    for (AreaFragment *f : FragmentsController::getController()->getSortedFragments())
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

void FragmentArea::on_btnReSort_clicked()
{
//    for (AreaFragment *fragment : FragmentsController::getController()->getUnsortedFragments())
//    {
//        fragment->rotateAng = Tool::get_suggest_rotation(Tool::QImageToMat(fragment->getOriginalImage()));
//        fragment->rotateAng *= -1;
//        fragment->update();
//    }
//    update();
    lock.lock();
    resortPosition();
    lock.unlock();
}

void FragmentArea::resortPosition()
{
    qInfo() << "click resort";
    std::vector<AreaFragment *> fragments = FragmentsController::getController()->getUnsortedFragments();
    double maxWidth = 0;
    double maxHeight = 0;
    for (AreaFragment *fragment : fragments)
    {
        cv::Mat mat = Tool::QImageToMat(fragment->getOriginalImage());
        cv::Mat offset;
        Tool::rotateAndOffset(mat, Tool::getRotationMatrix(mat.cols / 2.0, mat.rows / 2.0, Tool::angToRad(fragment->rotateAng)), offset);
        maxWidth = std::max(maxWidth, 1.0 * mat.cols);
        maxHeight = std::max(maxHeight, 1.0 * mat.rows);
    }
    maxWidth /= 1.5;
    maxHeight /= 1.5;
    int side = std::ceil(std::sqrt(fragments.size()));
    double scall = (std::max)(this->rect().width() / (1.0 * maxWidth * (side + 1)), this->rect().height() / (1.0 * maxHeight * (side + 1)));
    MainWindow::mainWindow->setImageSize(scall);
    for (int i = 0; i < fragments.size(); ++i)
    {
        int x = maxWidth * 1.0 * (i % side) * scall;
        int y = maxHeight * 1.0 * (i / side) * scall;
        AreaFragment *fragment = fragments[i];
        fragment->rotateAng = 0;
        fragment->setPos(x, y);
    }
    update();
    std::vector<TransMatAndConfi> relatedPieces = RefreshThread::getRelatedPieces();
    vector<int> stillPieces;
    stillPieces.push_back(0);
    while (true)
    {
        TransMatAndConfi maxConfiTransMat;
        maxConfiTransMat.confidence = -1.0;
        for (int stillP : stillPieces)
        {
            for (const TransMatAndConfi &relatedPiece : relatedPieces)
            {
                auto iterOther = std::find(stillPieces.begin(), stillPieces.end(), relatedPiece.otherFrag);
                if (stillP == relatedPiece.thisFrag && iterOther == stillPieces.end())
                {
                    maxConfiTransMat = relatedPiece;
                }
            }
        }
        if (maxConfiTransMat.confidence <= -1.0)
            for (const TransMatAndConfi &relatedPiece : relatedPieces)
            {
                auto iterOther = std::find(stillPieces.begin(), stillPieces.end(), relatedPiece.otherFrag);
                if (iterOther == stillPieces.end())
                {
                    maxConfiTransMat = relatedPiece;
                    break;
                }
            }
        if (maxConfiTransMat.confidence <= -1.0) break;
        stillPieces.push_back(maxConfiTransMat.otherFrag);
        AreaFragment *fragment1 = FragmentsController::getController()->findFragmentById(maxConfiTransMat.thisFrag);
        AreaFragment *fragment2 = FragmentsController::getController()->findFragmentById(maxConfiTransMat.otherFrag);
        if (fragment1 == nullptr || fragment2 == nullptr) continue;
        if (fragment1 == fragment2) continue;
        const Piece *p1 = findPieceById(fragment1->getPieces(), maxConfiTransMat.thisFrag);
        const Piece *p2 = findPieceById(fragment2->getPieces(), maxConfiTransMat.otherFrag);
        if (p1 == nullptr || p2 == nullptr) continue;
        HintFragment::moveRelatedPieceToPos(p1, p2, maxConfiTransMat.transMat);
    }
    for (AreaFragment *f : FragmentsController::getController()->getUnsortedFragments())
    {
        f->clearMovedSign();
    }
}
