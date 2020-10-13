#ifndef REFRESHTHREAD_H
#define REFRESHTHREAD_H

#include <QThread>
#include <fragmentscontroller.h>
#include <ui/hintwindow.h>
#include <ui/fragmentarea.h>
#include <QtDebug>
#include <QMutex>
#include <areafragment.h>


class HintWindow;
class HintFragment;

class GetResThread : public QThread
{
public:
    GetResThread(std::vector<TransMatAndConfi> *confiMat, const int &pieceID);
    virtual void run() override;

private:

    int pieceID;
    std::vector<TransMatAndConfi> *confiMats;
};

class RefreshThread : public QThread
{
    Q_OBJECT

public:
    RefreshThread(AreaFragment *const fragment);

    void stopThread();

    virtual void run() override;

signals:
    void deleteOldFragments();
    void setNewFragments();

private:
    int getPieceIDX(std::vector<Piece> pieces, const int &id);
    void setHint(const std::vector<TransMatAndConfi> &resConfiMat);

public:
    static int confidence;

private:
    AreaFragment *fragment;
    FragmentsController *fragCtrl;
    static QMutex setFragmentLocker;
    std::vector<GetResThread *> allThreads;
    bool stoped;
};


#endif // REFRESHTHREAD_H
