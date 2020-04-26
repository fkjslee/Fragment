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

MainWindow *MainWindow::mainWindow = nullptr;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    FragmentsController::getController()->createAllFragments("./MIT_1/");
    ui->setupUi(this);
    fragCtrl = FragmentsController::getController();
    connect(ui->desktop->getScene(), &EventGraphicsScene::removeFragment, ui->fragmentArea, &FragmentArea::update);

    createMenu();
    mainWindow = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update()
{
    ui->desktop->update();
    ui->fragmentArea->update();
    on_imageSizeController_valueChanged(ui->imageSizeController->value());
    QMainWindow::update();
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
    menuFile->addAction(actOpen);

    actSave = new QAction(tr("save"), this);
    actSave->setShortcut(Qt::CTRL | Qt::Key_S);
    menuFile->addAction(actSave);

    actClose = new QAction(tr("close"), this);
    actClose->setShortcut(Qt::CTRL | Qt::Key_W);
    menuFile->addAction(actClose);
    ui->menubar->addMenu(menuFile);


    menuEdit = new QMenu(tr("edit"), this);
    actCopy = new QAction(tr("copy"), this);
    actCopy->setShortcut(Qt::CTRL | Qt::Key_C);
    menuEdit->addAction(actCopy);

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
    std::vector<FragmentUi *> unsortedFragments = fragCtrl->getUnsortedFragments();
    for (FragmentUi *fragment : unsortedFragments)
    {
        fragment->scaledToWidth(1.0 * value / 100);
    }
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
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("open file"));
    fileDialog->setDirectory("./");
    fileDialog->setFileMode(QFileDialog::Directory);
//    fileDialog->setViewMode(QFileDialog::List);
//    qDebug() << fileDialog->getExistingDirectory();
//    FragmentsController::getController()->createAllFragments(fileDialog->getExistingDirectory());
//    QStringList filter;
//    filter << "Image files (*.png *.jpg)";
//    qDebug() << filter;
//    fileDialog->setNameFilters(filter);
//    QStringList fileNames;
//    if (fileDialog->exec())
//    {
//        fileNames = fileDialog->selectedFiles();
//    }

}

