#ifndef HINTWINDOW_H
#define HINTWINDOW_H

#include <QWidget>
#include "fragmentui.h"
#include <hintscene.h>
#include <QThread>
#include <Tool.h>
#include <network.h>
#include <QtDebug>
#include <fragmentscontroller.h>
#include <ui/refreshthread.h>

namespace Ui
{
    class HintWindow;
}

class RefreshThread;

struct HintFragment {
public:
    HintFragment() {}
    FragmentUi* fragJoint;
    FragmentUi* fragBeJointed;
    FragmentUi* fragInHintWindow;
    int p1ID;
    int p2ID;
    cv::Mat transMat;

    bool operator == (const HintFragment& rhs) const {
        return this == &rhs;
    }
};

class HintWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HintWindow(QWidget *parent = nullptr);
    ~HintWindow();
    static HintWindow* getHintWindow() {
        return hintWindow;
    }

private:

public slots:
    void on_btnAutoJoint_clicked();

    void actSuggestTrigged();
    void deleteOldFragments();
    void setNewFragments();

public:
    std::vector<HintFragment> hintFragments;
    static unsigned int maxHintSize;


private slots:
    void on_btnClearAI_clicked();

    void on_refreshBtn_clicked();

private:
    Ui::HintWindow *ui;
    HintScene *scene;
    std::vector<HintFragment> getSelecetHintFrags();
    std::vector<RefreshThread*> threads;
    static HintWindow* hintWindow;
};

#endif // HINTWINDOW_H
