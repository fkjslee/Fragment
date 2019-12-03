#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <iostream>

HintWindow::HintWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HintWindow)
{
    std::cout << "hint window construted" << std::endl;
    ui->setupUi(this);
}

HintWindow::~HintWindow()
{
    delete ui;
}
