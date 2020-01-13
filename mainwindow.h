#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QTranslator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    static bool getCtlStatus() { return keyCtlOn; }

signals:
    void update();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void on_imageSizeController_valueChanged(int value);
    void changeLanguageToCN();
    void changeLanguageToEN();

private:
    Ui::MainWindow *ui;
    static bool keyCtlOn;
    QMenu* menuFile;
    QAction* actNew;
    QAction* actOpen;
    QAction* actSave;
    QAction* actClose;

    QMenu* menuEdit;
    QAction* actCopy;
    QAction* actUndo;
    QAction* actRedo;

    QMenu* menuTool;
    QMenu* menuLanguage;
    QAction* actChinese;
    QAction* actEnglish;
};
#endif // MAINWINDOW_H
