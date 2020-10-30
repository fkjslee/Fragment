#ifndef HINTWINDOW_H
#define HINTWINDOW_H

#include <QWidget>
#include <hintscene.h>
#include <QThread>
#include <Tool.h>
#include <network.h>
#include <QtDebug>
#include <fragmentscontroller.h>
#include <ui/refreshthread.h>
#include <hintfragment.h>

namespace Ui
{
    class HintWindow;
}

class RefreshThread;

struct SuggestedFragment
{
public:
    SuggestedFragment() {}
    AreaFragment *fragCorrToArea;
    AreaFragment *fragCorrToHint;
    HintFragment *selectedFragment;
    int p1ID;
    int p2ID;
    cv::Mat transMat;

    bool operator == (const SuggestedFragment &rhs) const
    {
        return this == &rhs;
    }
};

class HintWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HintWindow(QWidget *parent = nullptr);
    ~HintWindow();
    static HintWindow *getHintWindow()
    {
        return hintWindow;
    }

    SuggestedFragment getSuggestedFragmentByHintFragment(const HintFragment *const hintFragment);
    void randomSuggestFragment();

public slots:
    void on_btnAutoJoint_clicked();

    void actSuggestTrigged();
    void deleteOldFragments();
    void setNewFragments();

public:
    std::vector<SuggestedFragment> suggestedFragments;
    static unsigned int maxHintSize;


private slots:
    void on_btnClearAI_clicked();

    void on_refreshBtn_clicked();

    void on_btnFixedPosition_clicked();

    void suggestFragment(AreaFragment *areaFragment, bool needShow = true);

private:
    Ui::HintWindow *ui;
    HintScene *scene;
    std::vector<SuggestedFragment> getSelecetSuggestFrags();
    std::vector<RefreshThread *> threads;
    static HintWindow *hintWindow;
};

#endif // HINTWINDOW_H
