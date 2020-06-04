#include "refreshthread.h"
#include <cstdlib>


int RefreshThread::confidence = int(100 * 0.9 + 0.5);
QMutex RefreshThread::setFragmentLocker;
RefreshThread::RefreshThread(FragmentUi* const fragment) {
    this->fragment = fragment;
    fragCtrl = FragmentsController::getController();
    stoped = false;
}

void RefreshThread::stopThread()
{
    stoped = true;
}

int RefreshThread::getPieceID(std::vector<Piece> pieces, QString name){
    for (int i = 0; i < (int)pieces.size(); ++i)
        if (pieces[i].pieceID == name)
            return i;
    return -1;
}

void RefreshThread::run()
{
    std::vector<TransMatAndConfi> allConfiMats[MAX_FRAGMENT_NUM];
    for (Piece p : fragment->getPieces()) {
        GetResThread* thread = new GetResThread(&allConfiMats[p.pieceID.toInt()], p.pieceID);
        thread->start();
        allThreads.emplace_back(thread);
    }

    for (GetResThread* thread : allThreads) {
        thread->wait();
    }

    std::vector<TransMatAndConfi> resConfiMat;
    for (int i = 0; i < MAX_FRAGMENT_NUM; ++i)
        for (int j = 0; j < (int)allConfiMats[i].size(); ++j)
            resConfiMat.emplace_back(allConfiMats[i][j]);


    int random = rand() % 100;
    if (random < confidence)
        sort(resConfiMat.begin(), resConfiMat.end());
    else
        sort(resConfiMat.rbegin(), resConfiMat.rend());

    if(!stoped) {
        setFragmentLocker.lock();
        setHint(resConfiMat);
        HintWindow::getHintWindow()->deleteOldFragments();
        HintWindow::getHintWindow()->setNewFragments();
        setFragmentLocker.unlock();
    }
}

void RefreshThread::setHint(const std::vector<TransMatAndConfi>& resConfiMat)
{
    HintWindow* hintWindow = HintWindow::getHintWindow();
    for (const TransMatAndConfi& confiMat : resConfiMat) {
        FragmentUi* anotherFragment = fragCtrl->findFragmentByName(QString::number(confiMat.otherFrag));
        if (anotherFragment == nullptr) {
            qInfo() << "another fragment " + QString::number(confiMat.otherFrag) << " not in the work area";
            continue;
        }

        const int p1 = getPieceID(fragment->getPieces(), QString::number(confiMat.thisFrag));
        const int p2 = getPieceID(anotherFragment->getPieces(), QString::number(confiMat.otherFrag));

        struct HintFragment hintFrag;
        hintFrag.fragJoint = fragment;
        hintFrag.fragBeJointed = anotherFragment;
        hintFrag.fragInHintWindow = new FragmentUi(anotherFragment->getPieces(), anotherFragment->getOriginalImage(), "mirror " + anotherFragment->getFragmentName() + " to " + fragment->getFragmentName(), Platfrom::HintArea);
        hintFrag.p1ID = p1;
        hintFrag.p2ID = p2;
        hintFrag.transMat = confiMat.transMat.clone();
        std::vector<HintFragment> vec = hintWindow->hintFragments;
        bool fragInHint = false;
        for (HintFragment f : vec) {
            if (f.fragBeJointed == hintFrag.fragBeJointed && f.fragJoint == hintFrag.fragJoint) {
                fragInHint = true;
            }
        }
        if (fragInHint) continue;

        bool fragInFragmentArea = Tool::checkFragInFragmentArea(hintFrag.fragJoint);
        if (Tool::checkFragInFragmentArea(hintFrag.fragBeJointed) == false) fragInFragmentArea = false;
        if (hintFrag.fragJoint == hintFrag.fragBeJointed) fragInFragmentArea = false;
        if (!fragInFragmentArea) continue;

        if (hintWindow->hintFragments.size() >= HintWindow::maxHintSize)
            hintWindow->hintFragments.erase(hintWindow->hintFragments.begin());
        hintWindow->hintFragments.emplace_back(hintFrag);
        break;
    }
}

GetResThread::GetResThread(std::vector<TransMatAndConfi>* confiMats, const QString& pieceName)
{
    this->pieceName = pieceName;
    this->confiMats = confiMats;
}

void GetResThread::run()
{
    QString res = Network::sendMsg("a " + pieceName);
    res.replace("[", "");
    res.replace("]", "");
    res.replace("\n", "");
    QStringList msgList = res.split(" ");
    QStringList msgList2;
    for (QString s : msgList)
        if (s != "")
            msgList2.append(s);
    for (int i = 0; i < msgList2.length(); i += 11) {
        cv::Mat transMat(3, 3, CV_32FC1);
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                transMat.at<float>(j, k) = msgList2[i+2+(j*3+k)].toFloat();
        transMat = Tool::normalToOpencvTransMat(transMat);

        TransMatAndConfi confiMat;
        confiMat.thisFrag = pieceName.toInt();
        confiMat.otherFrag = msgList[i].toInt();
        confiMat.confidence = msgList[i+1].toFloat();
        confiMat.transMat = transMat.clone();
        confiMats->emplace_back(confiMat);
    }
}
