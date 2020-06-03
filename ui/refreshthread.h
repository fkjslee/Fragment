#ifndef REFRESHTHREAD_H
#define REFRESHTHREAD_H

#include <QThread>
#include <ui/fragmentui.h>
#include <fragmentscontroller.h>
#include <ui/hintwindow.h>
#include <ui/fragmentarea.h>
#include <QtDebug>
#include <QMutex>


class HintWindow;
struct HintFragment;

class GetResThread : public QThread {
public:
    GetResThread(QString* res, std::vector<TransMatAndConfi>* confiMat, const QString& pieceName);
    virtual void run() override;

private:
    QString* res;
    QString pieceName;
    std::vector<TransMatAndConfi>* confiMats;
};

class RefreshThread : public QThread {
    Q_OBJECT

public:
    RefreshThread() {}
    RefreshThread(FragmentUi* const fragment, HintWindow* hintWindow);

    virtual void run() override;

signals:
    void deleteOldFragments();
    void setNewFragments();

private:
    int getPieceID(std::vector<Piece> pieces, QString name);
    void setHint(const std::vector<TransMatAndConfi>& resConfiMat);

private:
    FragmentUi* fragment;
    FragmentsController* fragCtrl;
    HintWindow* hintWindow;
    static QMutex setFragmentLocker;
};


#endif // REFRESHTHREAD_H
