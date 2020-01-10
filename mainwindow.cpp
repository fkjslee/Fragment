#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMetaEnum>

bool MainWindow::keyCtlOn = false;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Fragment::createAllFragments("./fragment2/");
    ui->setupUi(this);
    connect(ui->desktop->getScene(), &EventGraphicsScene::removeFragment, ui->fragmentArea, &FragmentArea::update);
    connect(this, &MainWindow::update, ui->fragmentArea, &FragmentArea::update);
    ui->checkBtn->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Control)
        keyCtlOn = true;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Control)
        keyCtlOn = false;
}

void MainWindow::on_imageSizeController_valueChanged(int value)
{
    std::vector<Fragment*> unsortedFragments = Fragment::getUnsortedFragments();
    for (Fragment* fragment : unsortedFragments) {
        fragment->scaledToWidth(1.0 * value / 100);
    }
    emit update();
}

void MainWindow::on_checkBtn_clicked()
{
    qDebug() << "check : " << keyCtlOn;
}
