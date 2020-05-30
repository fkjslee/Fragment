#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <set>
#include <QMessageBox>
#include <ui/fragmentui.h>

HintWindow* HintWindow::hintWindow = nullptr;
unsigned int HintWindow::maxHintSize = 5;
namespace {
int getPieceID(std::vector<Piece> pieces, QString name) {
    for (int i = 0; i < (int)pieces.size(); ++i)
        if (pieces[i].pieceName == name)
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
    on_refreshBtn_clicked();
}

HintWindow::~HintWindow()
{
    for (int i = 0; i < (int)this->threads.size(); ++i) {
        threads[i]->quit();
        threads[i]->wait();
        delete threads[i];
    }
    delete scene;
    delete ui;
}

void HintWindow::deleteOldFragments()
{
    for (QGraphicsItem* item : scene->items()) {
        bool has = false;
        for (HintFragment hintFrag : hintFragments) {
            if (hintFrag.fragInHintWindow == item) {
                has = true;
                break;
            }
        }
        if (!has) {
            scene->removeItem(item);
            delete item;
        }
    }

}

void HintWindow::setNewFragments()
{
    QRect windowRect = this->rect();
    int N = int(hintFragments.size());
    for (int i = 0; i < N; ++i)
    {
        HintFragment hintFrag = hintFragments[unsigned(i)];
        FragmentUi* fragment = hintFrag.fragInHintWindow;
        fragment->setPos(0, windowRect.top() + windowRect.height() * i / N);
        bool has = false;
        for (QGraphicsItem* item : scene->items()) {
            if (item == fragment) {
                has = true;
                break;
            }
        }
        if (has) continue;
        scene->addItem(fragment);
    }
    update();
}

void HintWindow::on_refreshBtn_clicked()
{
    FragmentsController* fragCtrl = FragmentsController::getController();
    std::vector<FragmentUi*> refreshFragments;
    for (FragmentUi* f : fragCtrl->getUnsortedFragments()) {
        if (f->isSelected()) {
            refreshFragments.emplace_back(f);
        }
    }

    for (FragmentUi* f : refreshFragments) {
        for (Piece p : f->getPieces()) {
            RefreshThread* thread = new RefreshThread(f, p.pieceName, this);
            thread->start();
            this->threads.push_back(thread);
        }
    }
}

void HintWindow::on_btnAutoJoint_clicked()
{
    FragmentsController* fragCtrl = FragmentsController::getController();

    auto selectHintFrags = getSelecetHintFrags();
    if (selectHintFrags.size() != 1) {
        QMessageBox::critical(nullptr, QObject::tr("auto joint error"), QObject::tr("please choose one fragments to auto joint"),
                              QMessageBox::Cancel);
        return;
    }
    HintFragment selectFrag = selectHintFrags[0];
    bool exist = Tool::checkFragInFragmentArea(selectFrag.fragJoint);
    if (Tool::checkFragInFragmentArea(selectFrag.fragBeJointed) == false) exist = false;
    if (selectFrag.fragJoint == selectFrag.fragBeJointed) exist = false;
    if (!exist) {
        QMessageBox::information(nullptr, QObject::tr("joint error"), QObject::tr("选中的碎片已经不存在，可能已经被拼接?"),
                              QMessageBox::Cancel);
        return;
    }
    fragCtrl->jointFragment(selectFrag.fragJoint, selectFrag.p1ID, selectFrag.fragBeJointed, selectFrag.p2ID, selectFrag.transMat);
    on_refreshBtn_clicked();
}

std::vector<HintFragment> HintWindow::getSelecetHintFrags()
{
    std::vector<HintFragment> res;
    for (HintFragment hintFrag : hintFragments) {
        if (hintFrag.fragInHintWindow->isSelected()) {
            res.emplace_back(hintFrag);
        }
    }
    return res;
}

void HintWindow::on_btnClearAI_clicked()
{
    for (int i = 0; i < (int)this->threads.size(); ++i) {
        threads[i]->quit();
        threads[i]->wait();
        delete threads[i];
    }
    threads.clear();
    hintFragments.clear();
    deleteOldFragments();
    update();
}
