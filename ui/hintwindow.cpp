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
}

HintWindow::~HintWindow()
{
    for (int i = 0; i < (int)this->threads.size(); ++i)
    {
        if (threads[i]->isFinished() == false) threads[i]->cancelThread();
        threads[i]->wait();
        delete threads[i];
    }
    delete scene;
    delete ui;
}

SuggestedFragment HintWindow::getSuggestedFragmentByHintFragment(const HintFragment *const hintFragment)
{
    SuggestedFragment pressedFragment;
    for (const SuggestedFragment &f : HintWindow::getHintWindow()->suggestedFragments)
    {
        if (f.selectedFragment == hintFragment)
            pressedFragment = f;
    }
    return pressedFragment;
}

void HintWindow::randomSuggestFragment()
{
    int minSuggestSize = 1e9;
    AreaFragment *minSuggFragment = nullptr;
    for (AreaFragment *areaFragment : FragmentsController::getController()->getUnsortedFragments())
    {
        int fragmentSuggNum = 0;
        for (const Piece &p : areaFragment->getPieces())
        {
            fragmentSuggNum += RefreshThread::getAllConfiMat()[p.pieceID].size();
        }
        if (minSuggestSize > fragmentSuggNum)
        {
            minSuggestSize = fragmentSuggNum;
            minSuggFragment = areaFragment;
        }
    }

    for (RefreshThread *thread : this->threads)
    {
        thread->cancelThread();
        thread->wait();
    }
    threads.clear();

    suggestFragment(minSuggFragment, false);
}

void HintWindow::deleteOldFragments()
{
    FragmentsController *fragCtrl = FragmentsController::getController();
    for (QGraphicsItem *item : scene->items())
    {
        bool fragMirrorInHint = false;
        for (SuggestedFragment hintFrag : suggestedFragments)
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
        for (const SuggestedFragment &suggFrag : suggestedFragments)
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
                Tool::eraseInVector(suggestedFragments, suggFrag);
                break;
            }
        }
    }
    update();
}

void HintWindow::setNewFragments()
{
    QRect windowRect = this->rect();
    int N = int(suggestedFragments.size());
    for (int i = 0; i < N; ++i)
    {
        SuggestedFragment hintFrag = suggestedFragments[unsigned(i)];
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

    for (RefreshThread *thread : this->threads)
    {
        thread->cancelThread();
        thread->wait();
    }
    threads.clear();

    suggestFragment(refreshFragment, true);
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
    SuggestedFragment selectFrag = selectHintFrags[0];
    bool exist = FragmentsController::getController()->checkFragInFragmentArea(selectFrag.fragCorrToArea);
    if (FragmentsController::getController()->checkFragInFragmentArea(selectFrag.fragCorrToHint) == false) exist = false;
    if (selectFrag.fragCorrToArea == selectFrag.fragCorrToHint) exist = false;
    if (!exist)
    {
        QMessageBox::information(nullptr, QObject::tr("joint error"), QObject::tr("选中的碎片已经不存在，可能已经被拼接?"),
                                 QMessageBox::Cancel);
        return;
    }
    fragCtrl->jointFragment(selectFrag.fragCorrToArea, selectFrag.p1, selectFrag.fragCorrToHint, selectFrag.p2, selectFrag.transMat);
    deleteOldFragments();
}

std::vector<SuggestedFragment> HintWindow::getSelecetSuggestFrags()
{
    std::vector<SuggestedFragment> res;
    for (SuggestedFragment hintFrag : suggestedFragments)
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
        if (threads[i]->isFinished() == false) threads[i]->cancelThread();
    }
    threads.clear();
    suggestedFragments.clear();
    deleteOldFragments();
    update();
}

void HintWindow::on_refreshBtn_clicked()
{
    update();
}

void HintWindow::on_btnFixedPosition_clicked()
{
    for (SuggestedFragment pressedFragment : getSelecetSuggestFrags())
    {
        bool exist = FragmentsController::getController()->checkFragInFragmentArea(pressedFragment.fragCorrToArea);
        if (FragmentsController::getController()->checkFragInFragmentArea(pressedFragment.fragCorrToHint) == false) exist = false;
        if (pressedFragment.fragCorrToArea == pressedFragment.fragCorrToHint) exist = false;
        if (!exist)
        {
            continue;
        }

        cv::Mat trans = pressedFragment.p2->transMat.inv(); // jointed fragent back to start position

        trans = pressedFragment.transMat * trans; // jointed fragment fusion with jointing fragment
        trans = pressedFragment.p2->transMat * trans; // joing fragment back to start position

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

void HintWindow::suggestFragment(AreaFragment *areaFragment, bool needShow)
{
    if (areaFragment == nullptr) return;
    RefreshThread *thread = new RefreshThread(areaFragment, needShow);
    thread->start();
    this->threads.push_back(thread);
}
