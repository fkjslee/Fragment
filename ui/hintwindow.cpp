#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <set>
#include <QMessageBox>

HintWindow *HintWindow::hintWindow = nullptr;
unsigned int HintWindow::maxHintSize = 5;

HintWindow::HintWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HintWindow)
{
    ui->setupUi(this);
    hintWindow = this;
    scene = new HintScene;
    ui->view->setScene(scene);
    scene->setBackgroundBrush(QColor(128, 128, 128));
    actSuggestTrigged();
}

HintWindow::~HintWindow()
{
    for (int i = 0; i < (int)this->threads.size(); ++i)
    {
        if (threads[i]->isFinished() == false) threads[i]->stopThread();
        threads[i]->wait();
        delete threads[i];
    }
    delete scene;
    delete ui;
}

SuggestFragment HintWindow::getSuggestFragmentByHintFragment(const HintFragment *const hintFragment)
{
    SuggestFragment pressedFragment;
    pressedFragment.p1ID = -1;
    for (const SuggestFragment &f : HintWindow::getHintWindow()->suggestFragments)
    {
        if (f.selectedFragment == hintFragment)
            pressedFragment = f;
    }
    return pressedFragment;
}

void HintWindow::deleteOldFragments()
{
    FragmentsController *fragCtrl = FragmentsController::getController();
    for (QGraphicsItem *item : scene->items())
    {
        bool fragMirrorInHint = false;
        for (SuggestFragment hintFrag : suggestFragments)
        {
            if (hintFrag.selectedFragment == item)
            {
                fragMirrorInHint = true;
                break;
            }
        }
        if (!fragMirrorInHint)
        {
            scene->removeItem(item);
            delete item;
        }
    }
    update();

    bool fragUpdate = true;
    while(fragUpdate)
    {
        fragUpdate = false;
        for (const SuggestFragment &suggFrag : suggestFragments)
        {
            bool fragJointInFragArea = false;
            bool fragBeJointInFragArea = false;
            for (AreaFragment *f : fragCtrl->getUnsortedFragments())
            {
                if (suggFrag.fragCorrToArea == f)
                {
                    fragJointInFragArea = true;
                }
                if (suggFrag.fragCorrToHint == f)
                {
                    fragBeJointInFragArea = true;
                }
            }
            if (!fragJointInFragArea || !fragBeJointInFragArea)
            {
                if (scene->items().contains(suggFrag.selectedFragment))
                {
                    scene->removeItem(suggFrag.selectedFragment);
                }
                fragUpdate = true;
                Tool::eraseInVector(suggestFragments, suggFrag);
                break;
            }
        }
    }
    update();
}

void HintWindow::setNewFragments()
{
    QRect windowRect = this->rect();
    int N = int(suggestFragments.size());
    for (int i = 0; i < N; ++i)
    {
        SuggestFragment hintFrag = suggestFragments[unsigned(i)];
        HintFragment *fragment = hintFrag.selectedFragment;
        fragment->setPos(0, windowRect.top() + windowRect.height() * i / N);
        bool has = false;
        for (QGraphicsItem *item : scene->items())
        {
            if (item == fragment)
            {
                has = true;
                break;
            }
        }
        if (has) continue;
        scene->addItem(fragment);
    }
    update();
}

void HintWindow::actSuggestTrigged()
{
    FragmentsController *fragCtrl = FragmentsController::getController();
    AreaFragment *refreshFragment = nullptr;
    for (AreaFragment *f : fragCtrl->getUnsortedFragments())
    {
        if (f->isSelected())
        {
            refreshFragment = f;
            break;
        }
    }

    if (refreshFragment == nullptr)
        return;

    for (RefreshThread *thread : this->threads)
    {
        thread->stopThread();
    }

    RefreshThread *thread = new RefreshThread(refreshFragment);
    thread->start();
    this->threads.push_back(thread);
}

void HintWindow::on_btnAutoJoint_clicked()
{
    qInfo() << "click btn auto joint";
    FragmentsController *fragCtrl = FragmentsController::getController();

    auto selectHintFrags = getSelecetSuggestFrags();
    if (selectHintFrags.size() != 1)
    {
        QMessageBox::critical(nullptr, QObject::tr("auto joint error"), QObject::tr("please choose one fragments to auto joint"),
                              QMessageBox::Cancel);
        return;
    }
    SuggestFragment selectFrag = selectHintFrags[0];
    bool exist = FragmentsController::getController()->checkFragInFragmentArea(selectFrag.fragCorrToArea);
    if (FragmentsController::getController()->checkFragInFragmentArea(selectFrag.fragCorrToHint) == false) exist = false;
    if (selectFrag.fragCorrToArea == selectFrag.fragCorrToHint) exist = false;
    if (!exist)
    {
        QMessageBox::information(nullptr, QObject::tr("joint error"), QObject::tr("选中的碎片已经不存在，可能已经被拼接?"),
                                 QMessageBox::Cancel);
        return;
    }
    fragCtrl->jointFragment(selectFrag.fragCorrToArea, selectFrag.p1ID, selectFrag.fragCorrToHint, selectFrag.p2ID, selectFrag.transMat);
    deleteOldFragments();
}

std::vector<SuggestFragment> HintWindow::getSelecetSuggestFrags()
{
    std::vector<SuggestFragment> res;
    for (SuggestFragment hintFrag : suggestFragments)
    {
        if (hintFrag.selectedFragment->isSelected())
        {
            res.emplace_back(hintFrag);
        }
    }
    return res;
}

void HintWindow::on_btnClearAI_clicked()
{
    for (int i = 0; i < (int)this->threads.size(); ++i)
    {
        if (threads[i]->isFinished() == false) threads[i]->stopThread();
    }
    threads.clear();
    suggestFragments.clear();
    deleteOldFragments();
    update();
}

void HintWindow::on_refreshBtn_clicked()
{
    update();
}

void HintWindow::on_btnFixedPosition_clicked()
{
    for (SuggestFragment pressedFragment : getSelecetSuggestFrags())
    {
        bool exist = FragmentsController::getController()->checkFragInFragmentArea(pressedFragment.fragCorrToArea);
        if (FragmentsController::getController()->checkFragInFragmentArea(pressedFragment.fragCorrToHint) == false) exist = false;
        if (pressedFragment.fragCorrToArea == pressedFragment.fragCorrToHint) exist = false;
        if (!exist)
        {
            continue;
        }

        cv::Mat trans = pressedFragment.fragCorrToHint->getPieces()[pressedFragment.p2ID].transMat.inv(); // jointed fragent back to start position

        trans = pressedFragment.transMat * trans; // jointed fragment fusion with jointing fragment
        trans = pressedFragment.fragCorrToArea->getPieces()[pressedFragment.p1ID].transMat * trans; // joing fragment back to start position

        cv::Mat areaImg = Tool::QImageToMat(pressedFragment.fragCorrToArea->getOriginalImage());
        cv::Mat hadRotated = Tool::getRotationMatrix(areaImg.cols / 2.0, areaImg.rows / 2.0, Tool::angToRad(pressedFragment.fragCorrToArea->rotateAng));

        trans = Tool::getFirst3RowsMat(hadRotated) * trans; // jointed fragment move with jointing fragment
        trans = pressedFragment.fragCorrToArea->getOffsetMat() * trans; // add jointing fragment offset
        cv::Mat img = Tool::QImageToMat(pressedFragment.fragCorrToHint->getOriginalImage());
        double ang = std::acos(trans.at<float>(0, 0)) * 180.0 / CV_PI;
        if (trans.at<float>(0, 1) < 0) ang = 360.0 - ang;

        pressedFragment.fragCorrToHint->rotate(ang);
        trans = pressedFragment.fragCorrToHint->getOffsetMat().inv() * trans;

        pressedFragment.fragCorrToHint->setX(pressedFragment.fragCorrToArea->x() + trans.at<float>(0, 2));
        pressedFragment.fragCorrToHint->setY(pressedFragment.fragCorrToArea->y() + trans.at<float>(1, 2));
    }
}
