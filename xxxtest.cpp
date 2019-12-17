#include "xxxtest.h"
#include "ui_xxxtest.h"

xxxtest::xxxtest(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::xxxtest)
{
    ui->setupUi(this);
}

xxxtest::~xxxtest()
{
    delete ui;
}
