#include "desktop.h"
#include "ui_desktop.h"
#include <iostream>

Desktop::Desktop(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Desktop)
{
    ui->setupUi(this);
    std::cout << "desktop constructed" << std::endl;
}

Desktop::~Desktop()
{
    delete ui;
}
