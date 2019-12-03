#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fragment.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    std::cout << "main window constructed" << std::endl;
    ui->setupUi(this);
    ui->desktop->show();
    std::vector<Fragment>* fragments_unsorted = Fragment::fragments_unsorted;
    for(int i = 0; i < 9; ++i) {
        fragments_unsorted->push_back(Fragment(QString(i)));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

