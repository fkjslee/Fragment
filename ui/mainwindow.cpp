#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMetaEnum>
#include <QMessageBox>
#include <Tool.h>
#include <fragmentscontroller.h>
#include <QUndoStack>
#include <CommonHeader.h>
#include <QDir>
#include <QWheelEvent>

#define noRecommendation 0
#define singleRecommendation 1
#define parallelRecommendation 2

MainWindow *MainWindow::mainWindow = nullptr;
int MainWindow::expMode = parallelRecommendation;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FragmentsController::getController()->createAllFragments("./mixed2/");
    fragCtrl = FragmentsController::getController();

    createMenu();
    mainWindow = this;
    ui->storeArea->hide();
    if (expMode == 0)
        ui->hintWindow->hide();
    FragmentArea::getFragmentArea()->on_btnReSort_clicked();
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update()
{
    on_imageSizeController_valueChanged(ui->imageSizeController->value());
    ui->storeArea->update();
    ui->fragmentArea->update();
    QMainWindow::update();
}

int MainWindow::getZoomSize()
{
    return ui->imageSizeController->value();
}

void MainWindow::setImageSize(double scall)
{
    ui->imageSizeController->setValue(scall * 100);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0 && ctrlPress)
    {
        ui->imageSizeController->setValue(ui->imageSizeController->value() + ui->imageSizeController->singleStep());
    }
    else if (event->delta() < 0 && ctrlPress)
    {
        ui->imageSizeController->setValue(ui->imageSizeController->value() - ui->imageSizeController->singleStep());
    }
    else
    {
        QMainWindow::wheelEvent(event);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        ctrlPress = true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control)
    {
        ctrlPress = false;
    }
}

void MainWindow::createMenu()
{
    menuFile = new QMenu(tr("file"), this);
    actNew = new QAction(tr("new"), this);
    actNew->setShortcut(Qt::CTRL | Qt::Key_Q);
    connect(actNew, &QAction::triggered, this, &MainWindow::triggerNew);
    menuFile->addAction(actNew);

    actOpen = new QAction(tr("open file"), this);
    actOpen->setShortcut(Qt::CTRL | Qt::Key_O);
//    menuFile->addAction(actOpen);

    actSave = new QAction(tr("save"), this);
    actSave->setShortcut(Qt::CTRL | Qt::Key_S);
    menuFile->addAction(actSave);

    actClose = new QAction(tr("close"), this);
    actClose->setShortcut(Qt::CTRL | Qt::Key_W);
    connect(actClose, &QAction::triggered, this, &MainWindow::triggerClose);
    menuFile->addAction(actClose);
    ui->menubar->addMenu(menuFile);


    menuEdit = new QMenu(tr("edit"), this);
    actCopy = new QAction(tr("copy"), this);
    actCopy->setShortcut(Qt::CTRL | Qt::Key_C);
//    menuEdit->addAction(actCopy);

    QUndoStack *undoStack = CommonHeader::undoStack;
    actUndo = undoStack->createUndoAction(this, tr("undo"));
    actUndo->setShortcut(Qt::CTRL | Qt::Key_Z);
    menuEdit->addAction(actUndo);

    actRedo = undoStack->createRedoAction(this, tr("redo"));
    actRedo->setShortcut(Qt::CTRL | Qt::Key_Y);
    menuEdit->addAction(actRedo);
    ui->menubar->addMenu(menuEdit);

    menuTool = new QMenu(tr("tool"), this);
    menuLanguage = new QMenu(tr("language"), this);
    menuTool->addMenu(menuLanguage);
    actChinese = new QAction(tr("Chinese"), this);
    menuLanguage->addAction(actChinese);
    connect(actChinese, &QAction::triggered, this, &MainWindow::changeLanguageToCN);
    actEnglish = new QAction(tr("English"), this);
    menuLanguage->addAction(actEnglish);
    connect(actEnglish, &QAction::triggered, this, &MainWindow::changeLanguageToEN);
    ui->menubar->addMenu(menuTool);
}

void MainWindow::on_imageSizeController_valueChanged(int value)
{
    std::vector<AreaFragment *> unsortedFragments = fragCtrl->getUnsortedFragments();
    AreaFragment *baseFragment = unsortedFragments[0];
    for (int i = 0; i < unsortedFragments.size(); ++i)
    {
        AreaFragment *fragment = unsortedFragments[i];
        fragment->scaledToWidth(1.0 * value / 100);
        double scaller;
        if (preImgSize == -1)
        {
            scaller = 1.0 * value / 100;
        }
        else
        {
            scaller = (1.0 * value / 100) / (1.0 * preImgSize / 100);
        }
        float newX = baseFragment->x() + (fragment->x() - baseFragment->x()) * scaller;
        float newY = baseFragment->y() + (fragment->y() - baseFragment->y()) * scaller;
        fragment->setPos(QPointF(newX, newY));
    }
    preImgSize = value;
}

void MainWindow::changeLanguageToCN()
{
    changeLanguage("CHS");
}

void MainWindow::changeLanguageToEN()
{
    changeLanguage("EN");
}

void MainWindow::changeLanguage(QString language)
{
    QFile configFile("config.txt");
    if (!configFile.exists())
    {
        QMessageBox::warning(nullptr, QObject::tr("file error!"), QObject::tr("config file not exist!"),
                             QMessageBox::Cancel);
        return;
    }
    else if (!configFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr, QObject::tr("read error!"), QObject::tr("config file can't read!"),
                             QMessageBox::Cancel);
        return;
    }

    QJsonObject config = Tool::stringToJsonObj(configFile.readAll());
    config["language"] = language;

    configFile.close();
    configFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    configFile.write(Tool::jsonObjToString(config).toUtf8());
    configFile.close();
    QMessageBox::information(nullptr, QObject::tr("language"), QObject::tr("set new language, please restart."),
                             QMessageBox::Ok);
}

void MainWindow::triggerNew()
{
    FragmentsController::getController()->createAllFragments(QFileDialog::getExistingDirectory());
}

void MainWindow::triggerClose()
{
    qApp->quit();
}

