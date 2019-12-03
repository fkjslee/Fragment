#include "fragmentarea.h"
#include "ui_fragmentarea.h"
#include <iostream>

FragmentArea::FragmentArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FragmentArea)
{
    ui->setupUi(this);
    std::cout << "fragment area constructed" << std::endl;
}

FragmentArea::~FragmentArea()
{
    delete ui;
}
