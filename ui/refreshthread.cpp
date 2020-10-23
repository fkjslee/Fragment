#include "refreshthread.h"
#include <cstdlib>
#include <hintfragment.h>


int RefreshThread::confidence = int(100 * 0.6 + 0.5);
QMutex RefreshThread::setFragmentLocker;
std::vector<TransMatAndConfi> RefreshThread::allConfiMats[MAX_FRAGMENT_NUM];
RefreshThread::RefreshThread(AreaFragment *fragment): fragment(fragment)
{
    fragCtrl = FragmentsController::getController();
    stoped = false;
    HintWindow *hintWindow = HintWindow::getHintWindow();
    connect(this, &RefreshThread::deleteOldFragments, hintWindow, &HintWindow::deleteOldFragments, Qt::ConnectionType::BlockingQueuedConnection);
    connect(this, &RefreshThread::setNewFragments, hintWindow, &HintWindow::setNewFragments, Qt::ConnectionType::BlockingQueuedConnection);
    connect(this, &RefreshThread::finished, this, &RefreshThread::stopThread);
    connect(this, &RefreshThread::updateFragment, fragment, &AreaFragment::updateFragment);
}

void RefreshThread::startThread()
{
    stoped = false;
    fragment->startToCalc();
    emit updateFragment();
}

void RefreshThread::stopThread()
{
    stoped = true;
    fragment->endToCalc();
    emit updateFragment();
}

int RefreshThread::getPieceIDX(std::vector<Piece> pieces, const int &id)
{
    for (int i = 0; i < (int)pieces.size(); ++i)
        if (pieces[i].pieceID == id)
            return i;
    return -1;
}

void RefreshThread::run()
{
    startThread();
    int minSuggestSize = 1e9;
    const Piece *minSuggPiece;
    for (const Piece &p : fragment->getPieces())
    {
        if (minSuggestSize > allConfiMats[p.pieceID].size())
        {
            minSuggestSize = allConfiMats[p.pieceID].size();
            minSuggPiece = &p;
        }
    }

    std::vector<TransMatAndConfi> resConfiMat;
    for (const Piece &p : fragment->getPieces())
    {
        for (int j = 0; j < allConfiMats[minSuggPiece->pieceID].size(); ++j)
            resConfiMat.emplace_back(allConfiMats[p.pieceID][j]);
    }

    if(!stoped)
    {
        setFragmentLocker.lock();
        setHint(resConfiMat);
        emit deleteOldFragments();
        emit setNewFragments();
        setFragmentLocker.unlock();
    }

    GetResThread *thread = new GetResThread(&allConfiMats[minSuggPiece->pieceID], minSuggPiece->pieceID);
    thread->start();
    thread->wait();

    resConfiMat.clear();
    for (const Piece &p : fragment->getPieces())
    {
        for (int j = 0; j < allConfiMats[p.pieceID].size(); ++j)
            resConfiMat.emplace_back(allConfiMats[p.pieceID][j]);
    }

    if(!stoped)
    {
        setFragmentLocker.lock();
        setHint(resConfiMat);
        emit deleteOldFragments();
        emit setNewFragments();
        setFragmentLocker.unlock();
    }
}

void RefreshThread::setHint(const std::vector<TransMatAndConfi> &resConfiMat)
{
    HintWindow *hintWindow = HintWindow::getHintWindow();
    hintWindow->suggestFragments.clear();
    for (const TransMatAndConfi &confiMat : resConfiMat)
    {
        AreaFragment *anotherFragment = fragCtrl->findFragmentById(confiMat.otherFrag);
        if (anotherFragment == nullptr)
        {
            qInfo() << "another fragment " + QString::number(confiMat.otherFrag) << " not in the work area";
            continue;
        }

        const int p1 = getPieceIDX(fragment->getPieces(), confiMat.thisFrag);
        const int p2 = getPieceIDX(anotherFragment->getPieces(), confiMat.otherFrag);

        struct SuggestFragment suggFrag;
        suggFrag.fragCorrToArea = fragment;
        suggFrag.fragCorrToHint = anotherFragment;
        suggFrag.selectedFragment = new HintFragment(anotherFragment->getPieces(), anotherFragment->getOriginalImage(), "mirror " + anotherFragment->getFragmentName() + " to " + fragment->getFragmentName());
        suggFrag.p1ID = p1;
        suggFrag.p2ID = p2;
        suggFrag.transMat = confiMat.transMat.clone();

        bool fragInFragmentArea = FragmentsController::getController()->checkFragInFragmentArea(suggFrag.fragCorrToArea);
        if (FragmentsController::getController()->checkFragInFragmentArea(suggFrag.fragCorrToHint) == false) fragInFragmentArea = false;
        if (suggFrag.fragCorrToArea == suggFrag.fragCorrToHint) fragInFragmentArea = false;
        if (!fragInFragmentArea) continue;

        if (hintWindow->suggestFragments.size() >= HintWindow::maxHintSize)
            break;

        hintWindow->suggestFragments.emplace_back(suggFrag);
    }
}

GetResThread::GetResThread(std::vector<TransMatAndConfi> *confiMats, const int &pieceID)
{
    this->pieceID = pieceID;
    this->confiMats = confiMats;
}

void GetResThread::run()
{
    QString res = Network::sendMsg(QString("a %1").arg(pieceID));
    res.replace("[", "");
    res.replace("]", "");
    res.replace("\n", "");
    QStringList msgList = res.split(" ");
    QStringList msgList2;
    for (QString s : msgList)
        if (s != "")
            msgList2.append(s);
    for (int i = 0; i < msgList2.length(); i += 11)
    {
        cv::Mat transMat(3, 3, CV_32FC1);
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                transMat.at<float>(j, k) = msgList2[i + 2 + (j * 3 + k)].toFloat();
        transMat = Tool::normalToOpencvTransMat(transMat);

        TransMatAndConfi confiMat;
        confiMat.thisFrag = pieceID;
        confiMat.otherFrag = msgList[i].toInt();
        confiMat.confidence = msgList[i + 1].toFloat();
        confiMat.transMat = transMat.clone();
        bool find = false;
        for (int j = 0; j < confiMats->size(); ++j)
        {
            if (confiMats->at(j).otherFrag == confiMat.otherFrag)
                find = true;
        }
        if (!find)
        {
            confiMats->emplace_back(confiMat);
            break;
        }
    }
}
