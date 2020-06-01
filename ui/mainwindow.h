#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QTranslator>
#include <fragmentscontroller.h>
#include <QKeyEvent>
#include <QFileDialog>


class FragmentsController;
namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    static MainWindow *mainWindow;
    void update();
    int getZoomSize();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void createMenu();

private slots:
    void on_imageSizeController_valueChanged(int value);
    void changeLanguageToCN();
    void changeLanguageToEN();
    void changeLanguage(QString language);
    void triggerNew();
    void triggerClose();

private:
    FragmentsController *fragCtrl;

    Ui::MainWindow *ui;
    QMenu *menuFile;
    QAction *actNew;
    QAction *actOpen;
    QAction *actSave;
    QAction *actClose;

    QMenu *menuEdit;
    QAction *actCopy;
    QAction *actUndo;
    QAction *actRedo;

    QMenu *menuTool;
    QMenu *menuLanguage;
    QAction *actChinese;
    QAction *actEnglish;

    QMenu *menuSetting;

    bool ctrlPress = false;

};
#endif // MAINWINDOW_H
