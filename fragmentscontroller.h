#ifndef FRAGMENTSCONTROLLER_H
#define FRAGMENTSCONTROLLER_H

#include <vector>
#include <ui/fragmentui.h>
#include <QGraphicsScene>
#include <ui/fragmentarea.h>
#include <ui/mainwindow.h>
#include <opencv2/opencv.hpp>
#include <areafragment.h>

class FragmentArea;
class MainWindow;


class FragmentsController : public QObject
{
public:
    void createAllFragments(const QString &fragmentsPath);
    static FragmentsController *getController();
    bool splitSelectedFragments();
    const std::vector<AreaFragment *> getSelectedFragments();
    std::vector<AreaFragment *> &getUnsortedFragments();
    std::vector<AreaFragment *> &getSortedFragments();
    AreaFragment *findFragmentById(const int &id);
    bool jointFragment(AreaFragment *f1, const int piece1ID, AreaFragment *f2, const int piece2ID, const cv::Mat &transMat);
    void selectFragment();
    void getGroundTruth(const QString &path);
    float calcScore();

public slots:
    void unSelectFragment();
    bool checkFragInFragmentArea(AreaFragment *frag);

private:
    FragmentsController();
    void initBgColor(const QString &fragmentPath);
    void clearAllFrgments();

private:
    static FragmentsController *controller;

    std::vector<AreaFragment *> sortedFragments;
    std::vector<AreaFragment *> unsortedFragments;
    std::vector<cv::Mat> groundTruth;

    std::vector<unsigned int> bgColor;
};

#endif // FRAGMENTSCONTROLLER_H
