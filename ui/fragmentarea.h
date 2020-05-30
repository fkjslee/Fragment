#ifndef FRAGMENTAREA_H
#define FRAGMENTAREA_H

#include <QWidget>
#include <ui/fragmentui.h>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <fragmentscontroller.h>
#include <QUndoStack>
#include <fragmentsscene.h>

namespace Ui
{
    class FragmentArea;
}

class FragmentsController;

class FragmentArea : public QWidget
{
    Q_OBJECT

public:
    explicit FragmentArea(QWidget *parent = nullptr);
    ~FragmentArea();
    virtual void update();
    static FragmentArea* getFragmentArea() {
        return fragmentArea;
    }

    void updateFragmentsPos();
    void setRotateAng(int value);

public slots:
    void on_btnJoint_clicked();

    void on_btnSplit_clicked();

private slots:
    void on_sldRotate_valueChanged(int value);

    void on_btnJointForce_clicked();

private:
    bool jointCheck();

private:
    Ui::FragmentArea *ui;
    FragmentsScene *scene;
    std::vector<FragmentUi *> fragmentItems;
    FragmentsController *fragCtrl;
    QUndoStack *undoStack;
    static FragmentArea* fragmentArea;
    std::vector<cv::Mat> groundTruth;
};

#endif // FRAGMENTAREA_H
