#include "refreshthread.h"


QMutex RefreshThread::locker;
RefreshThread::RefreshThread(FragmentUi* const fragment, QString pieceName, HintWindow* hintWindow) {
    this->fragment = fragment;
    this->pieceName = pieceName;
    this->hintWindow = hintWindow;
    fragCtrl = FragmentsController::getController();
    connect(this, &RefreshThread::deleteOldFragments, hintWindow, &HintWindow::deleteOldFragments, Qt::ConnectionType::BlockingQueuedConnection);
    connect(this, &RefreshThread::setNewFragments, hintWindow, &HintWindow::setNewFragments, Qt::ConnectionType::BlockingQueuedConnection);
}

int RefreshThread::getPieceID(std::vector<Piece> pieces, QString name){
    for (int i = 0; i < (int)pieces.size(); ++i)
        if (pieces[i].pieceName == name)
            return i;
    return -1;
}

void RefreshThread::run()
{
    QString res = Network::sendMsg("a " + pieceName);
    if (res == "-1") {
       return;
    }
    locker.lock();
    setHint(res);
    emit deleteOldFragments();
    emit setNewFragments();
    locker.unlock();
}

void RefreshThread::setHint(QString res)
{
    res.replace("[", "");
    res.replace("]", "");
    res.replace("\n", "");
    QStringList msgList = res.split(" ");
    QStringList msgList2;
    for (QString s : msgList)
        if (s != "")
            msgList2.append(s);
    for (int i = 0; i < msgList2.length(); i += 10) {
        FragmentUi* anotherFragment = fragCtrl->findFragmentByName(msgList2[i]);
        if (anotherFragment == nullptr) {
            qInfo() << "another fragment " + msgList2[i] << " not in the work area";
            continue;
        }
        cv::Mat transMat(3, 3, CV_32FC1);
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                transMat.at<float>(j, k) = msgList2[i+1+(j*3+k)].toFloat();
        transMat = Tool::normalToOpencvTransMat(transMat);
        const int p1 = getPieceID(fragment->getPieces(), pieceName);
        const int p2 = getPieceID(anotherFragment->getPieces(), msgList2[i]);

        struct HintFragment hintFrag;
        hintFrag.fragJoint = fragment;
        hintFrag.fragBeJointed = anotherFragment;
        hintFrag.fragInHintWindow = new FragmentUi(anotherFragment->getPieces(), anotherFragment->getOriginalImage(), "mirror " + anotherFragment->getFragmentName() + " to " + fragment->getFragmentName(), Platfrom::HintArea);
        hintFrag.p1ID = p1;
        hintFrag.p2ID = p2;
        hintFrag.transMat = transMat.clone();
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
    }
}

