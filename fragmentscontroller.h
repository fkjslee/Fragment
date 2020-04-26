#ifndef FRAGMENTSCONTROLLER_H
#define FRAGMENTSCONTROLLER_H

//#define MINE

#include <vector>
#include <ui/fragmentui.h>
#include <QGraphicsScene>
#include <ui/fragmentarea.h>
#include <ui/mainwindow.h>
#include <NumCpp.hpp>
#include <opencv2/opencv.hpp>

enum JointMethod {leftRight, rightLeft, upDown, downUp};

struct JointFragment
{
    FragmentUi *item;
    JointMethod method;
    double absGrayscale;
    JointFragment(FragmentUi *item, JointMethod method, double absGrayscale)
        : item(item), method(method), absGrayscale(absGrayscale) {}
};

class FragmentArea;
class MainWindow;


class FragmentsController
{
public:
    void createAllFragments(const QString &fragmentsPath);
    bool createFragment(const QString &fragmentPath);
    static FragmentsController *getController();
    /* todo
     * Choose most possible fragment which could be jointed by giving fragment(item)
     */
    std::vector<JointFragment> getMostPossibleFragments(FragmentUi *item = nullptr);
    /* todo
     * get most possible jointsing method through giving fragment(f1 and f2)
     */
    JointFragment mostPossibleJointMethod(FragmentUi *f1, FragmentUi *f2);
    bool splitSelectedFragments();
    const std::vector<FragmentUi *> getSelectedFragments();
    std::vector<FragmentUi *> &getUnsortedFragments();
    std::vector<FragmentUi *> &getSortedFragments();
    FragmentUi* findFragmentByName(const QString& name);
#ifdef MINE
    bool jointFragment(FragmentUi *f1, JointFragment jointFragment);
#else
    bool jointFragment(FragmentUi *f1, FragmentUi *f2, const cv::Mat& transMat);
#endif
    void selectFragment();
    void unSelectFragment();

private:
    FragmentsController();

private:
    static FragmentsController *controller;

    std::vector<FragmentUi *> sortedFragments;
    std::vector<FragmentUi *> unsortedFragments;
};

#endif // FRAGMENTSCONTROLLER_H
