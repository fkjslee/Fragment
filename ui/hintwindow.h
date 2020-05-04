#ifndef HINTWINDOW_H
#define HINTWINDOW_H

#include <QWidget>
#include "fragmentui.h"
#include <hintscene.h>

namespace Ui
{
    class HintWindow;
}

struct HintFragment {
    FragmentUi* thisFrag;
    FragmentUi* originFrag;
    FragmentUi* showFrag;
    int p1ID;
    int p2ID;
    cv::Mat transMat;
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
    void deleteOldFragments();
    void getNewFragments();
    void setNewFragments();

public slots:
    void on_btnAutoJoint_clicked();

    void on_refreshBtn_clicked();

private:
    Ui::HintWindow *ui;
    HintScene *scene;
    std::vector<HintFragment> hintFragments;
    std::vector<HintFragment> getSelecetHintFrags();
    static HintWindow* hintWindow;
};

#endif // HINTWINDOW_H
