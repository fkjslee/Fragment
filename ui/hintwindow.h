#ifndef HINTWINDOW_H
#define HINTWINDOW_H

#include <QWidget>
#include "eventgraphicsscene.h"
#include "fragmentui.h"

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

private:
    void deleteOldFragments();
    void getNewFragments();
    void setNewFragments();

private slots:
    void on_refreshBtn_clicked();

    void on_btnAutoJoint_clicked();

private:
    Ui::HintWindow *ui;
    EventGraphicsScene *scene;
    std::vector<HintFragment> hintFragments;
    std::vector<HintFragment> getSelecetHintFrags();
};

#endif // HINTWINDOW_H
