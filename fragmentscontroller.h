#ifndef FRAGMENTSCONTROLLER_H
#define FRAGMENTSCONTROLLER_H

#include <vector>
#include <ui/fragmentui.h>
#include <QGraphicsScene>
#include <ui/fragmentarea.h>
#include <ui/mainwindow.h>
#include <opencv2/opencv.hpp>

class FragmentArea;
class MainWindow;


class FragmentsController : public QObject
{
public:
    void createAllFragments(const QString &fragmentsPath);
    static FragmentsController *getController();
    bool splitSelectedFragments();
    const std::vector<FragmentUi *> getSelectedFragments();
    std::vector<FragmentUi *> &getUnsortedFragments();
    std::vector<FragmentUi *> &getSortedFragments();
    FragmentUi *findFragmentByName(const QString &name);
    bool jointFragment(FragmentUi *f1, const int piece1ID, FragmentUi *f2, const int piece2ID, const cv::Mat &transMat);
    void selectFragment();
    void getGroundTruth(const QString &path);
    float calcScore();

public slots:
    void unSelectFragment();

private:
    FragmentsController();
    void initBgColor(const QString &fragmentPath);
    void clearAllFrgments();

private:
    static FragmentsController *controller;

    std::vector<FragmentUi *> sortedFragments;
    std::vector<FragmentUi *> unsortedFragments;
    std::vector<cv::Mat> groundTruth;

    std::vector<unsigned int> bgColor;
};

#endif // FRAGMENTSCONTROLLER_H
