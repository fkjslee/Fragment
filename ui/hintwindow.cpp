#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <fragmentscontroller.h>
#include <set>
#include <fragmentscontroller.h>
#include <QtDebug>
#include <QMessageBox>
#include <network.h>

HintWindow* HintWindow::hintWindow = nullptr;
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
    delete scene;
    delete ui;
}

void HintWindow::deleteOldFragments()
{
    for (HintFragment hintFrag : hintFragments)
    {
        FragmentUi* fragment = hintFrag.showFrag;
        scene->removeItem(fragment);
        delete fragment;
    }
    hintFragments.clear();
}

void HintWindow::getNewFragments()
{
    FragmentsController* fragCtrl = FragmentsController::getController();
    auto unsortedFragments = fragCtrl->getUnsortedFragments();
    for (FragmentUi* f : unsortedFragments) {
        if (f->isSelected()) {
            for (const Piece& piece : f->getPieces()) {
                const QString& pieceName = piece.pieceName;
                QString res = Network::sendMsg("a " + pieceName);
                if (res == "-1") {
                    return;
                }
                res.replace("[", "");
                res.replace("]", "");
                res.replace("\n", "");
                QStringList msgList = res.split(" ");
                QStringList msgList2;
                for (QString s : msgList)
                    if (s != "")
                        msgList2.append(s);
                for (int i = 0; i < msgList2.length(); i += 10) {
                    // if two fragments in one peace, pass them
                    if (f->getFragmentName().split(" ").contains(msgList2[i])) {
                        continue;
                    }
                    FragmentUi* anotherFragment = fragCtrl->findFragmentByName(msgList2[i]);
                    if (anotherFragment == nullptr) {
                        qCritical() << "another fragment is null";
                        continue;
                    }
                    cv::Mat transMat(3, 3, CV_32FC1);
                    for (int j = 0; j < 3; ++j)
                        for (int k = 0; k < 3; ++k)
                            transMat.at<float>(j, k) = msgList2[i+1+(j*3+k)].toFloat();
                    const int p1 = getPieceID(f->getPieces(), pieceName);
                    const int p2 = getPieceID(anotherFragment->getPieces(), msgList2[i]);

                    HintFragment hintFrag;
                    hintFrag.thisFrag = f;
                    hintFrag.originFrag = anotherFragment;
                    hintFrag.showFrag = new FragmentUi(anotherFragment->getPieces(), anotherFragment->getOriginalImage(), "mirror " + anotherFragment->getFragmentName());
                    hintFrag.p1ID = p1;
                    hintFrag.p2ID = p2;
                    hintFrag.transMat = transMat.clone();
                    hintFragments.emplace_back(hintFrag);
                }
            }
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
        FragmentUi* fragment = hintFrag.showFrag;
        fragment->setPos(0, windowRect.top() + windowRect.height() * i / N);
        scene->addItem(fragment);
    }
    update();
}

void HintWindow::on_refreshBtn_clicked()
{
    deleteOldFragments();
    getNewFragments();
    setNewFragments();
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
    fragCtrl->jointFragment(selectFrag.thisFrag, selectFrag.p1ID, selectFrag.originFrag, selectFrag.p2ID, selectFrag.transMat);
}

std::vector<HintFragment> HintWindow::getSelecetHintFrags()
{
    std::vector<HintFragment> res;
    for (HintFragment hintFrag : hintFragments) {
        if (hintFrag.showFrag->isSelected()) {
            res.emplace_back(hintFrag);
        }
    }
    return res;
}
