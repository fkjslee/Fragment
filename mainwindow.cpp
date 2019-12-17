#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Fragment::createAllFragments("./fragment2/");
    ui->setupUi(this);
    connect(ui->desktop->getScene(), &EventGraphicsScene::removeFragment, ui->fragmentArea, &FragmentArea::update);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_imageSizeController_valueChanged(int value)
{
    std::vector<Fragment*> unsortedFragments = Fragment::getUnsortedFragments();
    for (Fragment* fragment : unsortedFragments) {
        fragment->scaledToWidth(1.0 * value / 100);
    }

}
