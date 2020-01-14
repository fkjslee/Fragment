#include "fragmentscontroller.h"
#include <Tool.h>
#include <QDir>

FragmentsController *FragmentsController::controller = nullptr;
FragmentsController::FragmentsController()
{
    vector<Fragment *> unsortedFragments = vector<Fragment *>();
    vector<Fragment *> sortedFragments = vector<Fragment *>();
    vector<Fragment *> chosenFragments = vector<Fragment *>();
}

void FragmentsController::createAllFragments(const QString &fragmentsPath)
{
    qInfo() << "createAllFragments";
    QDir dir(fragmentsPath);
    QStringList filter;
    filter << "*.jpg" << "*.png";
    QStringList nameList = dir.entryList(filter);
    int i = 0;
    for (const QString &fileName : nameList)
    {
        std::vector<Piece> vec;
        vec.push_back(Piece(dir.absolutePath() + "/" + fileName, QString("f%1").arg(++i)));
        unsortedFragments.emplace_back(new Fragment(vec, QImage(dir.absolutePath() + "/" + fileName), QString("f%1").arg(i)));
    }
}

FragmentsController *FragmentsController::getController()
{
    if (controller == nullptr)
        return controller = new FragmentsController;
    else
        return controller;
}

std::vector<JointFragment> FragmentsController::getMostPossibleFragments(Fragment *item)
{
    std::vector<JointFragment> res;
    if (item == nullptr)
    {
        std::vector<Fragment *> unsorted_fragments = getUnsortedFragments();
        for (Fragment *unsorted_fragment : unsorted_fragments)
        {
            res.emplace_back(JointFragment(unsorted_fragment, JointMethod::leftRight, 0));
            if (res.size() >= 5)
                break;
        }
    }
    else
    {
        JointFragment minFragment(nullptr, JointMethod::leftRight, INFINITE);
        for (Fragment *otherFragment : getUnsortedFragments())
        {
            if (item == otherFragment) continue;
            JointFragment possilbeFragment = mostPossibleJointMethod(item, otherFragment);
            if (possilbeFragment.absGrayscale < minFragment.absGrayscale)
                minFragment = possilbeFragment;
        }
        if (minFragment.item != nullptr)
            res.emplace_back(minFragment);
    }
    return res;
}

JointFragment FragmentsController::mostPossibleJointMethod(Fragment *f1, Fragment *f2)
{
    JointFragment minFragment(nullptr, JointMethod::leftRight, INFINITE);
    const cv::Mat &m1 = Tool::QImageToMat(f1->getOriginalImage());
    const cv::Mat &m2 = Tool::QImageToMat(f2->getOriginalImage());
    double absGrayscale;
    absGrayscale = Tool::calcLeftRightAbsGrayscale(m1, m2);
    if (absGrayscale < minFragment.absGrayscale)
        minFragment = JointFragment(f2, JointMethod::leftRight, absGrayscale);

    absGrayscale = Tool::calcLeftRightAbsGrayscale(m2, m1);
    if (absGrayscale < minFragment.absGrayscale)
        minFragment = JointFragment(f2, JointMethod::rightLeft, absGrayscale);

    absGrayscale = Tool::calcUpDownAbsGrayscale(m1, m2);
    if (absGrayscale < minFragment.absGrayscale)
        minFragment = JointFragment(f2, JointMethod::upDown, absGrayscale);

    absGrayscale = Tool::calcUpDownAbsGrayscale(m2, m1);
    if (absGrayscale < minFragment.absGrayscale)
        minFragment = JointFragment(f2, JointMethod::downUp, absGrayscale);
    return minFragment;
}

bool FragmentsController::splitSelectedFragments()
{
    for (Fragment *splitFragment : getSelectedFragments())
    {
        Tool::eraseInVector(unsortedFragments, splitFragment);
        for (Piece piece : splitFragment->getPiece())
        {
            std::vector<Piece> vec;
            vec.push_back(piece);
            unsortedFragments.emplace_back(new Fragment(vec, QImage(piece.piecePath), piece.pieceName));
        }
    }
    return true;
}

const std::vector<Fragment *> FragmentsController::getSelectedFragments()
{
    std::vector<Fragment *> selectedFragments;
    for (Fragment *f : unsortedFragments)
    {
        if (f->getSelected())
            selectedFragments.emplace_back(f);
    }
    return selectedFragments;
}

std::vector<Fragment *> FragmentsController::getUnsortedFragments()
{
    return unsortedFragments;
}

std::vector<Fragment *> FragmentsController::getSortedFragments()
{
    return sortedFragments;
}

bool FragmentsController::jointFragment(Fragment *f1, JointFragment jointFragment)
{
    const cv::Mat &m1 = Tool::QImageToMat(f1->getOriginalImage());
    const cv::Mat &m2 = Tool::QImageToMat(jointFragment.item->getOriginalImage());
    cv::Mat jointMat;
    switch(jointFragment.method)
    {
        case leftRight:
            if (m1.rows == m2.rows)
                cv::hconcat(m1, m2, jointMat);
            break;
        case rightLeft:
            if (m1.rows == m2.rows)
                cv::hconcat(m2, m1, jointMat);
            break;
        case upDown:
            if (m1.cols == m2.cols)
                cv::vconcat(m1, m2, jointMat);
            break;
        case downUp:
            if (m1.cols == m2.cols)
                cv::vconcat(m2, m1, jointMat);
            break;
    }
    if (jointMat.empty()) return false;
    Tool::eraseInVector(unsortedFragments, f1);
    Tool::eraseInVector(unsortedFragments, jointFragment.item);
    std::vector<Piece> pieces;
    for (Piece p : f1->getPiece())
        pieces.emplace_back(p);
    for (Piece p : jointFragment.item->getPiece())
        pieces.emplace_back(p);
    Fragment *newFragment = new Fragment(pieces, Tool::MatToQImage(jointMat), f1->getFragmentName() + " " + jointFragment.item->getFragmentName());
    newFragment->setPos(f1->scenePos());
    unsortedFragments.emplace_back(newFragment);
    qInfo() << "joint fragmens " << f1->getFragmentName() << " and " << jointFragment.item->getFragmentName() << " with absGrayscale = " << jointFragment.absGrayscale;
    return true;
}

void FragmentsController::reverseChosenFragment(Fragment *f)
{
    std::vector<Fragment *>::iterator iterF = std::find(chosenFragments.begin(), chosenFragments.end(), f);
    if (iterF == chosenFragments.end())
        chosenFragments.emplace_back(f);
    else
        chosenFragments.erase(iterF);
}
