#include "hintwindow.h"
#include "ui_hintwindow.h"
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
    for (Fragment* fragment : fragments) {
        scene->removeItem(fragment);
        delete fragment;
    }
    fragments.clear();

    std::vector<JointFragment> possilbleFragments = Fragment::getMostPossibleFragments(nullptr);
    for (JointFragment jointFragment : possilbleFragments) {
        Fragment* f = jointFragment.item;
        fragments.emplace_back(new Fragment(f->getOriginalImage(), "copy of " + f->getFragmentName()));
    }
    QRect windowRect = this->rect();
    int N = int(fragments.size());
    for (int i = 0; i < N; ++i) {
        Fragment* fragment = fragments[unsigned(i)];
        fragment->setPos(0, windowRect.top() + windowRect.height() * i / N);
        scene->addItem(fragment);
    }
}
