#include "fragmentscontroller.h"
#include <Tool.h>
#include <QDir>
#include <commands.h>
#include <CommonHeader.h>

using namespace cv;

FragmentsController *FragmentsController::controller = nullptr;
FragmentsController::FragmentsController()
{
}

void FragmentsController::createAllFragments(const QString &fragmentsPath)
{
    qInfo() << "createAllFragments " << fragmentsPath;
    QDir dir(fragmentsPath);
    QStringList filter;
    filter << "*.jpg" << "*.png";
    QStringList nameList = dir.entryList(filter);
    int i = 0;
    for (const QString &fileName : nameList)
    {
        std::vector<Piece> vec;
        vec.push_back(Piece(dir.absolutePath() + "/" + fileName));
        unsortedFragments.emplace_back(new FragmentUi(vec, QImage(dir.absolutePath() + "/" + fileName), QString("f%1").arg(i)));
    }
}

bool FragmentsController::createFragment(const QString &fragmentPath)
{
    std::vector<Piece> vec;
    vec.push_back(Piece(fragmentPath));
    unsortedFragments.emplace_back(new FragmentUi(vec, QImage(fragmentPath), vec[0].pieceName));
    return true;
}

FragmentsController *FragmentsController::getController()
{
    if (controller == nullptr)
        return controller = new FragmentsController;
    else
        return controller;
}

std::vector<JointFragment> FragmentsController::getMostPossibleFragments(FragmentUi *item)
{
    std::vector<JointFragment> res;
    if (item == nullptr)
    {
        std::vector<FragmentUi *> unsorted_fragments = getUnsortedFragments();
        for (FragmentUi *unsorted_fragment : unsorted_fragments)
        {
            res.emplace_back(JointFragment(unsorted_fragment, JointMethod::leftRight, 0));
            if (res.size() >= 5)
                break;
        }
    }
    else
    {
        JointFragment minFragment(nullptr, JointMethod::leftRight, INFINITE);
        for (FragmentUi *otherFragment : getUnsortedFragments())
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

JointFragment FragmentsController::mostPossibleJointMethod(FragmentUi *f1, FragmentUi *f2)
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

bool FragmentsController::splitSelectedFragments(FragmentArea *fragmentArea)
{
    std::vector<FragmentUi *> redoFragments;
    std::vector<FragmentUi *> undoFragments;
    for (FragmentUi *splitFragment : getSelectedFragments())
    {
        for (Piece piece : splitFragment->getPiece())
        {
            std::vector<Piece> vec;
            vec.push_back(piece);
            FragmentUi *newSplitFragment = new FragmentUi(vec, QImage(piece.piecePath), piece.pieceName);
            redoFragments.emplace_back(newSplitFragment);
        }
        undoFragments.emplace_back(splitFragment);
    }
    CommonHeader::undoStack->push(new SplitUndo(undoFragments, redoFragments, fragmentArea));
    return true;
}

const std::vector<FragmentUi *> FragmentsController::getSelectedFragments()
{
    std::vector<FragmentUi *> selectedFragments;
    for (FragmentUi *f : unsortedFragments)
    {
        if (f->getSelected())
            selectedFragments.emplace_back(f);
    }
    for (FragmentUi *f : sortedFragments)
    {
        if (f->getSelected())
            selectedFragments.emplace_back(f);
    }
    return selectedFragments;
}

std::vector<FragmentUi *> &FragmentsController::getUnsortedFragments()
{
    return unsortedFragments;
}

std::vector<FragmentUi *> &FragmentsController::getSortedFragments()
{
    return sortedFragments;
}

bool FragmentsController::jointFragment(FragmentUi *f1, JointFragment jointFragment, FragmentArea *fragmentArea)
{
    FragmentUi *f2 = jointFragment.item;
    const cv::Mat &m1 = Tool::QImageToMat(f1->getOriginalImage());
    const cv::Mat &m2 = Tool::QImageToMat(f2->getOriginalImage());
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
    std::vector<Piece> pieces;
    for (Piece p : f1->getPiece())
        pieces.emplace_back(p);
    for (Piece p : f2->getPiece())
        pieces.emplace_back(p);
    FragmentUi *newFragment = new FragmentUi(pieces, Tool::MatToQImage(jointMat), f1->getFragmentName() + " " + f2->getFragmentName());
    newFragment->setPos(QPoint((f1->scenePos().x() + f2->scenePos().x()) / 2, (f1->scenePos().y() + f2->scenePos().y()) / 2));
    newFragment->undoFragments.push_back(f1);
    newFragment->undoFragments.push_back(f2);
    std::vector<FragmentUi *> undoFragments;
    undoFragments.push_back(f1);
    undoFragments.push_back(f2);
    JointUndo *temp = new JointUndo(undoFragments, newFragment, fragmentArea);
    CommonHeader::undoStack->push(temp);
    qInfo() << "joint fragmens " << f1->getFragmentName() << " and " << f2->getFragmentName() << " with absGrayscale = " << jointFragment.absGrayscale;
    return true;
}

void FragmentsController::selectFragment(FragmentUi *f)
{
    if (!f) return;
    unsortedFragments.push_back(f);
    Tool::eraseInVector(sortedFragments, f);
    MainWindow::mainWindow->update();

}

void FragmentsController::unSelectFragment(FragmentUi *f)
{
    qDebug() << "size = " << FragmentsController::getController()->sortedFragments.size();
    if (!f) return;
    sortedFragments.push_back(f);
    Tool::eraseInVector(unsortedFragments, f);
    MainWindow::mainWindow->update();
}
