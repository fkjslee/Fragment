#ifndef FRAGMENTAREA_H
#define FRAGMENTAREA_H

#include <QWidget>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <fragmentscontroller.h>
#include <QUndoStack>
#include <fragmentsscene.h>
#include <areafragment.h>

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
    static FragmentArea *getFragmentArea()
    {
        return fragmentArea;
    }

    void updateFragmentsPos();
    void setRotateAng(int value);
    void resortPosition();

public slots:
    void on_btnSplit_clicked();

    void on_btnJointForce_clicked();

private slots:
    void on_sldRotate_valueChanged(int value);


    void on_btnReSort_clicked();

private:
    bool jointCheck();

private:
    Ui::FragmentArea *ui;
    FragmentsScene *scene;
    std::vector<AreaFragment *> fragmentItems;
    FragmentsController *fragCtrl;
    QUndoStack *undoStack;
    static FragmentArea *fragmentArea;
};

#endif // FRAGMENTAREA_H
