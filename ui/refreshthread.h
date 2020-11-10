#ifndef REFRESHTHREAD_H
#define REFRESHTHREAD_H

#include <QThread>
#include <fragmentscontroller.h>
#include <ui/hintwindow.h>
#include <ui/fragmentarea.h>
#include <QtDebug>
#include <QMutex>
#include <map>
#include <areafragment.h>


class HintWindow;
class HintFragment;

class RefreshThread : public QThread
{
    Q_OBJECT

public:
    RefreshThread(AreaFragment *fragment, bool needShow);

    void startThread();

    void completeThread();

    void cancelThread();

    void getRes(std::vector<TransMatAndConfi> *confiMats, const int &pieceID);

    static const std::vector<TransMatAndConfi> getRelatedPieces();

    static const std::vector<TransMatAndConfi> *getAllConfiMat()
    {
        return allConfiMats;
    }

    static bool judgePieceNeedSuggest(int pieceID)
    {
        return !suggestAllPieces[pieceID];
    }

    virtual void run() override;

    static int getConfidence()
    {
        return confidence;
    }

signals:
    void deleteOldFragments();
    void setNewFragments();
    void updateFragment();

private:
    int getPieceIDX(std::vector<Piece> pieces, const int &id);
    void setHint(const std::vector<TransMatAndConfi> &resConfiMat);

private:
    static int confidence;
    AreaFragment *fragment;
    FragmentsController *fragCtrl;
    static QMutex setFragmentLocker;
    static std::map<int, bool> suggestAllPieces;
    bool stoped;
    static std::vector<TransMatAndConfi> allConfiMats[MAX_FRAGMENT_NUM];
    bool needShow;
    int startTime = -1;
};


#endif // REFRESHTHREAD_H
