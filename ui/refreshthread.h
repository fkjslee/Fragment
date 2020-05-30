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
class RefreshThread : public QThread {
    Q_OBJECT

public:
    RefreshThread() {}
    RefreshThread(FragmentUi* const fragment, QString pieceName, HintWindow* hintWindow);

    virtual void run() override;

signals:
    void deleteOldFragments();
    void setNewFragments();

private:
    int getPieceID(std::vector<Piece> pieces, QString name);
    void setHint(QString res);

private:
    FragmentUi* fragment;
    FragmentsController* fragCtrl;
    QString pieceName;
    HintWindow* hintWindow;
    static QMutex locker;
};


#endif // REFRESHTHREAD_H
