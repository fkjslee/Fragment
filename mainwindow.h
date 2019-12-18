#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void on_imageSizeController_valueChanged(int value);

    void on_checkBtn_clicked();

private:
    Ui::MainWindow *ui;
    static bool keyCtlOn;
};
#endif // MAINWINDOW_H
