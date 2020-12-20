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

    void setRotateAng(int value, bool man = true);

public slots:
    void on_btnSplit_clicked();

    void on_btnJointForce_clicked();
    void on_btnReSort_clicked();

private slots:
    void on_sldRotate_valueChanged(int value);



private:
    bool jointCheck();
    void resortPosition();

private:
    Ui::FragmentArea *ui;
    FragmentsScene *scene;
    std::vector<AreaFragment *> fragmentItems;
    FragmentsController *fragCtrl;
    QUndoStack *undoStack;
    static FragmentArea *fragmentArea;
    static QMutex lock;
    bool manRotate = true;
};

#endif // FRAGMENTAREA_H
