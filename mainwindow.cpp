#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMetaEnum>
#include <QMessageBox>
#include <Tool.h>

bool MainWindow::keyCtlOn = false;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Fragment::createAllFragments("./fragment2/");
    ui->setupUi(this);
    connect(ui->desktop->getScene(), &EventGraphicsScene::removeFragment, ui->fragmentArea, &FragmentArea::update);
    connect(this, &MainWindow::update, ui->fragmentArea, &FragmentArea::update);

    menuFile = new QMenu(tr("file"), this);
    actNew = new QAction(tr("new"), this);
    actNew->setShortcut(Qt::CTRL | Qt::Key_N);
    menuFile->addAction(actNew);

    actOpen = new QAction(tr("open"), this);
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

    actUndo = new QAction(tr("undo"), this);
    actUndo->setShortcut(Qt::CTRL | Qt::Key_R);
    menuEdit->addAction(actUndo);

    actRedo = new QAction(tr("redo"), this);
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

void MainWindow::changeLanguageToCN()
{
    qDebug() << "trigger " << " change to CN";
    changeLanguage("CHS");
}

void MainWindow::changeLanguageToEN()
{
    qDebug() << "trigger " << " change to EN";
    changeLanguage("EN");
}

void MainWindow::changeLanguage(QString language)
{
    QFile configFile("config.txt");
    if (!configFile.exists()) {
        QMessageBox::warning(nullptr, QObject::tr("file error!"), QObject::tr("config file not exist!"),
                           QMessageBox::Cancel);
        return;
    } else if (!configFile.open(QIODevice::ReadOnly)) {
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
}
