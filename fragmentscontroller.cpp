#include "fragmentscontroller.h"
#include <Tool.h>
#include <QDir>
#include <commands.h>
#include <CommonHeader.h>
#pragma push_macro("slots")
#undef slots
#pragma pop_macro("slots")
#include <ctime>
#include <complex>
#define NO_IMPOET_ARRAY
#include <qrgb.h>
#include <ui/hintwindow.h>
#include <network.h>

namespace
{
    using namespace cv;
}

FragmentsController *FragmentsController::controller = nullptr;
FragmentsController::FragmentsController()
{
}

void FragmentsController::initBgColor(const QString &fragmentPath)
{
    const QString &path = fragmentPath + QDir::separator() + "bg_color.txt";
    QFile bgColorFile(path);
    bgColorFile.open(QIODevice::ReadOnly);

    QTextStream in(&bgColorFile);
    QString line = in.readLine();
    QStringList lines;
    while (!line.isNull())
    {
        lines.append(line);
        line = in.readLine();
    }

    bgColor.clear();
    for (QString line : lines)
    {
        QStringList colors = line.split(" ");
        bgColor.emplace_back(qRgb(colors[0].toInt(), colors[1].toInt(), colors[2].toInt()));
    }
    bgColor.emplace_back(qRgb(0, 0, 0));
}

void FragmentsController::clearAllFrgments()
{
    sortedFragments.clear();
    unsortedFragments.clear();
    if (MainWindow::mainWindow)
        MainWindow::mainWindow->update();
}

void FragmentsController::createAllFragments(const QString &fragmentsPath)
{
    clearAllFrgments();
    initBgColor(fragmentsPath);
    getGroundTruth(fragmentsPath);
    Network::loadTransMat(fragmentsPath);
    Network::loadGtMat(fragmentsPath);

    qInfo() << "createAllFragments " << fragmentsPath;
    QDir dir(fragmentsPath);
    QStringList filter;
    filter << "fragment*.jpg" << "fragment*.png";
    QStringList nameList = dir.entryList(filter);
    int i = 0;
    for (const QString &fileName : nameList)
    {
        std::vector<Piece> vec;
        vec.push_back(Piece(dir.absolutePath() + "/" + fileName, i));
        QImage img(dir.absolutePath() + "/" + fileName);
        for (auto bg_color : bgColor)
        {
            auto mask = img.createMaskFromColor(bg_color, Qt::MaskMode::MaskOutColor);
            img.setAlphaChannel(mask);
        }
        unsortedFragments.emplace_back(new AreaFragment(vec, img, QString("id = %1").arg(i)));
        ++i;
    }
    FragmentArea::getFragmentArea()->updateFragmentsPos();
    HintWindow::getHintWindow()->randomSuggestFragment();
    if (MainWindow::mainWindow) MainWindow::mainWindow->update();
}

FragmentsController *FragmentsController::getController()
{
    if (controller == nullptr)
        return controller = new FragmentsController;
    else
        return controller;
}

bool FragmentsController::splitSelectedFragments()
{
    std::vector<AreaFragment *> redoFragments;
    std::vector<AreaFragment *> undoFragments;
    for (AreaFragment *splitFragment : getSelectedFragments())
    {
        for (Piece oldP : splitFragment->getPieces())
        {
            Piece newP = oldP;
            newP.transMat = cv::Mat::eye(3, 3, CV_32FC1);
            std::vector<Piece> vec;
            vec.push_back(newP);
            QImage img(newP.piecePath);
            for (auto bg_color : bgColor)
            {
                auto mask = img.createMaskFromColor(bg_color, Qt::MaskMode::MaskOutColor);
                img.setAlphaChannel(mask);
            }
            AreaFragment *newSplitFragment = new AreaFragment(vec, img, QString("id = %1").arg(newP.pieceID));
            redoFragments.emplace_back(newSplitFragment);
        }
        undoFragments.emplace_back(splitFragment);
    }
    CommonHeader::undoStack->push(new SplitUndo(undoFragments, redoFragments));
    return true;
}

const std::vector<AreaFragment *> FragmentsController::getSelectedFragments()
{
    std::vector<AreaFragment *> selectedFragments;
    for (AreaFragment *f : unsortedFragments)
    {
        if (f->isSelected())
            selectedFragments.emplace_back(f);
    }
    for (AreaFragment *f : sortedFragments)
    {
        if (f->isSelected())
            selectedFragments.emplace_back(f);
    }
    return selectedFragments;
}

std::vector<AreaFragment *> &FragmentsController::getUnsortedFragments()
{
    return unsortedFragments;
}

std::vector<AreaFragment *> &FragmentsController::getSortedFragments()
{
    return sortedFragments;
}

AreaFragment *FragmentsController::findFragmentById(const int &id)
{
    for (AreaFragment *f : getUnsortedFragments())
    {
        for (const Piece &p : f->getPieces())
        {
            if (p.pieceID == id) return f;
        }
    }
    return nullptr;
}

bool FragmentsController::jointFragment(AreaFragment *f1, const Piece *ptr_p1, AreaFragment *f2, const Piece *ptr_p2, const cv::Mat &originTransMat)
{
    const Piece p1 = *ptr_p1;
    const Piece p2 = *ptr_p2;
    const cv::Mat p1transMat = p1.transMat.clone();
    const cv::Mat p2transMat = p2.transMat.clone();
    cv::Mat p2transInv = p2.transMat.clone();
    cv::invert(p2transMat, p2transInv);

    cv::Mat src = Tool::QImageToMat(f1->getOriginalImage()); // 8UC4
    cv::Mat dst = Tool::QImageToMat(f2->getOriginalImage());

    Mat finalTransMat =  p1transMat * originTransMat * p2transInv;

    cv::Mat offsetMat;

    cv::Mat resMat = Tool::fusionImage(src, dst, finalTransMat, offsetMat);

    qInfo() << "run FusionImage";

    std::vector<Piece> pieces;
    for (Piece p : f1->getPieces())
    {
        Piece newP = p;
        newP.transMat = p.transMat.clone();
        newP.transMat = offsetMat.clone() * newP.transMat;
        pieces.emplace_back(newP);
    }
    for (Piece p : f2->getPieces())
    {
        Piece newP = p;
        newP.transMat = p.transMat.clone();
        newP.transMat = offsetMat.clone() * finalTransMat.clone() * newP.transMat;
        pieces.emplace_back(newP);
    }
    AreaFragment *newFragment = new AreaFragment(pieces, Tool::Mat8UC4ToQImage(resMat), f1->getFragmentName() + " " + f2->getFragmentName());
    float newX = f1->scenePos().x() - offsetMat.at<float>(0, 2) / (100.0 / MainWindow::mainWindow->getZoomSize());
    float newY = f1->scenePos().y() - offsetMat.at<float>(1, 2) / (100.0 / MainWindow::mainWindow->getZoomSize());
    cv::Mat preRotateMat = Tool::getRotationMatrix(src.cols / 2.0, src.rows / 2.0, Tool::angToRad(f1->rotateAng));
    cv::Mat afterRotateMat = Tool::getRotationMatrix(resMat.cols / 2.0, resMat.rows / 2.0, Tool::angToRad(f1->rotateAng));

    cv::Mat newImgOffset;
    cv::Mat nothing = resMat.clone();
    Tool::rotateAndOffset(nothing, Tool::getFirst3RowsMat(afterRotateMat), newImgOffset);

    cv::Mat preOffset = f1->getOffsetMat().clone();

    float prePosX = preRotateMat.at<float>(0, 2) + preOffset.at<float>(0, 2);
    float prePosY = preRotateMat.at<float>(1, 2) + preOffset.at<float>(1, 2);
    float movePosX = offsetMat.at<float>(0, 2);
    float movePosY = offsetMat.at<float>(1, 2);
    float afterPosX = movePosX * afterRotateMat.at<float>(0, 0) + movePosY * afterRotateMat.at<float>(0, 1) + afterRotateMat.at<float>(0, 2);
    float afterPosY = movePosX * afterRotateMat.at<float>(1, 0) + movePosY * afterRotateMat.at<float>(1, 1) + afterRotateMat.at<float>(1, 2);
    afterPosX += newImgOffset.at<float>(0, 2);
    afterPosY += newImgOffset.at<float>(1, 2);

    prePosX *= MainWindow::mainWindow->getZoomSize() / 100.0f;
    prePosY *= MainWindow::mainWindow->getZoomSize() / 100.0f;
    afterPosX *= MainWindow::mainWindow->getZoomSize() / 100.0f;
    afterPosY *= MainWindow::mainWindow->getZoomSize() / 100.0f;

    newX = f1->scenePos().x() + (prePosX - afterPosX);
    newY = f1->scenePos().y() + (prePosY - afterPosY);

    newFragment->setPos(newX, newY);
    newFragment->rotateAng = f1->rotateAng;
    newFragment->undoFragments.push_back(f1);
    newFragment->undoFragments.push_back(f2);
    std::vector<AreaFragment *> undoFragments;
    undoFragments.push_back(f1);
    undoFragments.push_back(f2);
    JointUndo *temp = new JointUndo(undoFragments, newFragment);
    CommonHeader::undoStack->push(temp);
    qInfo() << "joint fragments " << p1.pieceID << " and " << p2.pieceID;
    return true;
}

void FragmentsController::selectFragment()
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (AreaFragment *f : FragmentsController::getController()->getSortedFragments())
        {
            if (f->isSelected())
            {
                changed = true;
                unsortedFragments.push_back(f);
                Tool::eraseInVector(sortedFragments, f);
                break;
            }
        }
    }
    MainWindow::mainWindow->update();
}

void FragmentsController::unSelectFragment()
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (AreaFragment *f : FragmentsController::getController()->getUnsortedFragments())
        {
            if (f->isSelected())
            {
                changed = true;
                sortedFragments.push_back(f);
                Tool::eraseInVector(unsortedFragments, f);
                break;
            }
        }
    }
    MainWindow::mainWindow->update();
}

bool FragmentsController::checkFragInFragmentArea(AreaFragment *frag)
{
    bool exist = false;
    for (AreaFragment *f : FragmentsController::getController()->getUnsortedFragments())
    {
        if (f == frag)
            exist = true;
    }
    return exist;
}

void FragmentsController::getGroundTruth(const QString &path)
{
    QFile gtFile(path + QDir::separator() + "groundTruth.txt");
    gtFile.open(QIODevice::ReadOnly);
    QTextStream in(&gtFile);
    QString line = in.readLine();
    QStringList lines;
    while (!line.isNull())
    {
        lines.append(line);
        line = in.readLine();
    }

    groundTruth.clear();
    for (int i = 0; i < lines.length(); i += 2)
    {
        int fragID = lines[i].toInt();
        if (fragID - 1 != i / 2)
        {
            qCritical() << "error groundTruth";
        }
        QStringList nums = lines[i + 1].replace("  ", " ").split(" ");
        cv::Mat transMat = cv::Mat::eye(3, 3, CV_32FC1);
        transMat.at<float>(0, 0) = nums[0].toFloat();
        transMat.at<float>(0, 1) = nums[1].toFloat();
        transMat.at<float>(0, 2) = nums[2].toFloat();
        transMat.at<float>(1, 0) = nums[3].toFloat();
        transMat.at<float>(1, 1) = nums[4].toFloat();
        transMat.at<float>(1, 2) = nums[5].toFloat();
        groundTruth.emplace_back(Tool::normalToOpencvTransMat(transMat));
    }
}

float FragmentsController::calcScore()
{
    float score = 0;
    for (AreaFragment *f : getUnsortedFragments())
    {
        auto pieces = f->getPieces();
        for (int i = 1; i < (int)pieces.size(); ++i)
        {
            cv::Mat trans = pieces[0].transMat.inv() * pieces[i].transMat;
            int p1 = pieces[0].pieceID;
            int p2 = pieces[i].pieceID;
            cv::Mat gt = groundTruth[p1].inv() * groundTruth[p2];
            float len = std::sqrt(std::pow(trans.at<float>(0, 2), 2) + std::pow(trans.at<float>(1, 2), 2));
            float len2 = std::sqrt(std::pow(gt.at<float>(0, 2), 2) + std::pow(gt.at<float>(1, 2), 2));
            float x, y, xx, yy;
            if (std::abs(len) > 1e-7)
            {
                x = trans.at<float>(0, 2) / len / 2.0;
                y = trans.at<float>(1, 2) / len / 2.0;
            }
            else
            {
                x = 0;
                y = 0;
            }
            if (std::abs(len2) > 1e-7)
            {
                xx = gt.at<float>(0, 2) / len2 / 2.0;
                yy = gt.at<float>(1, 2) / len2 / 2.0;
            }
            else
            {
                xx = 0;
                yy = 0;
            }
            float eachScore = 1.0 / 4 * std::pow(trans.at<float>(0, 0) / 2 - gt.at<float>(0, 0) / 2, 2) +
                              1.0 / 4 * std::pow(trans.at<float>(0, 1) / 2 - gt.at<float>(0, 1) / 2, 2) +
                              1.0 / 4 * std::pow(x - xx, 2) +
                              1.0 / 4 * std::pow(y - yy, 2);
            score += (1 - eachScore) * 100;
        }
    }
    return score;
}

AreaFragment *FragmentsController::getFragmentByPiece(const Piece *p)
{
    if (p == nullptr) return nullptr;
    for (AreaFragment *areaFragment : getUnsortedFragments())
    {
        for (const Piece &eachPiece : areaFragment->getPieces())
        {
            if (eachPiece.pieceID == p->pieceID)
                return areaFragment;
        }
    }
    return nullptr;
}
