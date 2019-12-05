#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fragment.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

