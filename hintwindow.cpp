#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <iostream>
#include <set>

HintWindow::HintWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HintWindow)
{
    std::cout << "hint window construted" << std::endl;
    ui->setupUi(this);
    scene = new EventGraphicsScene(EventGraphicsScene::hintArea);
    ui->view->setScene(scene);
    scene->setBackgroundBrush(QColor(128, 128, 128));
    on_refreshBtn_clicked();
}

HintWindow::~HintWindow()
{
    delete ui;
}

void HintWindow::on_refreshBtn_clicked()
{
    for (ColorItem* colorItem : colorItems) {
        scene->removeItem(colorItem);
    }
    colorItems.clear();

    std::vector<Fragment*> possilbleFragments = Fragment::getMostPossibleFragments(nullptr);
    for (Fragment* f : possilbleFragments) {
        colorItems.emplace_back(new ColorItem(f));
    }
    QRect windowRect = this->rect();
    int N = int(colorItems.size());
    for (int i = 0; i < N; ++i) {
        ColorItem* colorItem = colorItems[i];
        colorItem->setPos(0, windowRect.top() + windowRect.height() * i / N);
        scene->addItem(colorItem);
    }
}
