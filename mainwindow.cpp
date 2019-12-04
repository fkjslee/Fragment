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
    Fragment::createFragments();
}

MainWindow::~MainWindow()
{
    delete ui;
}

