#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <fragmentscontroller.h>
#include <set>

HintWindow::HintWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HintWindow)
{
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::hintArea);
    ui->view->setScene(scene);
    scene->setBackgroundBrush(QColor(128, 128, 128));
    on_refreshBtn_clicked();
}

HintWindow::~HintWindow()
{
    delete scene;
    delete ui;
}

void HintWindow::on_refreshBtn_clicked()
{
    for (FragmentUi *fragment : fragments)
    {
        scene->removeItem(fragment);
        delete fragment;
    }
    fragments.clear();

    std::vector<JointFragment> possilbleFragments = FragmentsController::getController()->getMostPossibleFragments(nullptr);
    for (JointFragment jointFragment : possilbleFragments)
    {
        FragmentUi *f = jointFragment.item;
//        fragments.emplace_back(new Fragment(f->getOriginalImage(), "copy of " + f->getFragmentName()));
    }
    QRect windowRect = this->rect();
    int N = int(fragments.size());
    for (int i = 0; i < N; ++i)
    {
        FragmentUi *fragment = fragments[unsigned(i)];
        fragment->setPos(0, windowRect.top() + windowRect.height() * i / N);
        scene->addItem(fragment);
    }
}
