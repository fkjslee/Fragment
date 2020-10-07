#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <set>
#include <QMessageBox>
#include <ui/fragmentui.h>

HintWindow *HintWindow::hintWindow = nullptr;
unsigned int HintWindow::maxHintSize = 5;
namespace
{
    int getPieceID(std::vector<Piece> pieces, QString name)
    {
        for (int i = 0; i < (int)pieces.size(); ++i)
            if (pieces[i].pieceID == name)
                return i;
        return -1;
    }
}

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

void HintWindow::deleteOldFragments()
{
    FragmentsController *fragCtrl = FragmentsController::getController();
    for (QGraphicsItem *item : scene->items())
    {
        bool fragMirrorInHint = false;
        for (SuggestFragment hintFrag : suggestFragments)
        {
            if (hintFrag.fragInHintWindow == item)
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
            for (FragmentUi *f : fragCtrl->getUnsortedFragments())
            {
                if (suggFrag.fragJoint == f)
                {
                    fragJointInFragArea = true;
                }
                if (suggFrag.fragBeJointed == f)
                {
                    fragBeJointInFragArea = true;
                }
            }
            if (!fragJointInFragArea || !fragBeJointInFragArea)
            {
                if (scene->items().contains(suggFrag.fragInHintWindow))
                {
                    scene->removeItem(suggFrag.fragInHintWindow);
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
        FragmentUi *fragment = hintFrag.fragInHintWindow;
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
    std::vector<AreaFragment *> refreshFragments;
    for (AreaFragment *f : fragCtrl->getUnsortedFragments())
    {
        if (f->isSelected())
        {
            refreshFragments.emplace_back(f);
        }
    }

    for (AreaFragment *f : refreshFragments)
    {
        f->startToCalc();
        MainWindow::mainWindow->update();
        RefreshThread *thread = new RefreshThread(f);
        thread->start();
        this->threads.push_back(thread);
    }
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
    bool exist = FragmentsController::getController()->checkFragInFragmentArea(selectFrag.fragJoint);
    if (FragmentsController::getController()->checkFragInFragmentArea(selectFrag.fragBeJointed) == false) exist = false;
    if (selectFrag.fragJoint == selectFrag.fragBeJointed) exist = false;
    if (!exist)
    {
        QMessageBox::information(nullptr, QObject::tr("joint error"), QObject::tr("选中的碎片已经不存在，可能已经被拼接?"),
                                 QMessageBox::Cancel);
        return;
    }
    fragCtrl->jointFragment(selectFrag.fragJoint, selectFrag.p1ID, selectFrag.fragBeJointed, selectFrag.p2ID, selectFrag.transMat);
    deleteOldFragments();
}

std::vector<SuggestFragment> HintWindow::getSelecetSuggestFrags()
{
    std::vector<SuggestFragment> res;
    for (SuggestFragment hintFrag : suggestFragments)
    {
        if (hintFrag.fragInHintWindow->isSelected())
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
