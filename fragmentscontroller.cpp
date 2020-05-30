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
#include <numpy/arrayobject.h>
#define NO_IMPOET_ARRAY
#include <qrgb.h>
#include <network.h>

namespace
{
    using namespace cv;

PyObject* mat8UC42numpy(const Mat& src) {
    npy_intp dims[3] = {src.rows, src.cols, 3};
    unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * src.rows * src.cols * 3);
    int p = 0;
    for (int i = 0; i < src.rows; ++i)
        for (int j = 0; j < src.cols; ++j)
            for (int k = 0; k < 3; ++k)
                data[p++] = src.at<Vec4b>(i, j)[k];
    return PyArray_SimpleNewFromData(3, dims, NPY_UBYTE, data);
}

PyObject* mat32FC12numpy(const Mat& src) {
    npy_intp dims[2] = {src.rows, src.cols};
    float* data = (float*)malloc(sizeof(float) * src.rows * src.cols);
    int p = 0;
    for (int i = 0; i < src.rows; ++i)
        for (int j = 0; j < src.cols; ++j)
            data[p++] = src.at<float>(i, j);
    return PyArray_SimpleNewFromData(2, dims, NPY_FLOAT, data);
}

cv::Mat pyObject2Mat(PyObject* obj, int type) {
    PyArrayObject* ret_array;
    PyArray_OutputConverter(obj, &ret_array);
    npy_intp *shape = PyArray_SHAPE(ret_array);
    Mat resMat(shape[0], shape[1], type, PyArray_DATA(ret_array));
    return resMat;
}

}

FragmentsController *FragmentsController::controller = nullptr;
FragmentsController::FragmentsController()
{
    initPython();
}

void FragmentsController::initPython()
{
    Py_Initialize();
    import_array();
    if ( !Py_IsInitialized() )
    {
        qCritical() << "initialize failed!\n";
        return;
    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    static PyObject* pModule = PyImport_ImportModule("FusionImage");
    if (!pModule) {
        qCritical() << ("Cant open python file!\n");
        return;
    }

    funcFusionImage= PyObject_GetAttrString(pModule,"callFusionImage");
    if(!funcFusionImage){
        qCritical() << "Get function failed" << endl;
        return;
    }
}

void FragmentsController::initBgColor(const QString& fragmentPath)
{
    const QString& path = fragmentPath + QDir::separator() + "bg_color.txt";
    QFile bgColorFile(path);
    bgColorFile.open(QIODevice::ReadOnly);
    const QString& bgColorStr = QString(bgColorFile.readAll());
    QStringList colors = bgColorStr.split(" ");
    bgColor = qRgb(colors[0].toInt(), colors[1].toInt(), colors[2].toInt());
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

    qInfo() << "createAllFragments " << fragmentsPath;
    QDir dir(fragmentsPath);
    QStringList filter;
    filter << "fragment*.jpg" << "fragment*.png";
    QStringList nameList = dir.entryList(filter);
    int i = 0;
    for (const QString &fileName : nameList)
    {
        std::vector<Piece> vec;
        vec.push_back(Piece(dir.absolutePath() + "/" + fileName, QString("%1").arg(i)));
        QImage img(dir.absolutePath() + "/" + fileName);
        auto mask = img.createMaskFromColor(bgColor, Qt::MaskMode::MaskOutColor);
        img.setAlphaChannel(mask);
        unsortedFragments.emplace_back(new FragmentUi(vec, img, QString("%1").arg(i)));
        ++i;
    }
    FragmentArea::getFragmentArea()->updateFragmentsPos();
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
    std::vector<FragmentUi *> redoFragments;
    std::vector<FragmentUi *> undoFragments;
    for (FragmentUi *splitFragment : getSelectedFragments())
    {
        for (Piece oldP : splitFragment->getPieces())
        {
            Piece newP = oldP;
            newP.transMat = cv::Mat::eye(3, 3, CV_32FC1);
            std::vector<Piece> vec;
            vec.push_back(newP);
            QImage img(newP.piecePath);
            auto mask = img.createMaskFromColor(bgColor, Qt::MaskMode::MaskOutColor);
            img.setAlphaChannel(mask);
            FragmentUi *newSplitFragment = new FragmentUi(vec, img, newP.pieceName);
            redoFragments.emplace_back(newSplitFragment);
        }
        undoFragments.emplace_back(splitFragment);
    }
    CommonHeader::undoStack->push(new SplitUndo(undoFragments, redoFragments));
    return true;
}

const std::vector<FragmentUi *> FragmentsController::getSelectedFragments()
{
    std::vector<FragmentUi *> selectedFragments;
    for (FragmentUi *f : unsortedFragments)
    {
        if (f->isSelected())
            selectedFragments.emplace_back(f);
    }
    for (FragmentUi *f : sortedFragments)
    {
        if (f->isSelected())
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

FragmentUi *FragmentsController::findFragmentByName(const QString &name)
{
    for (FragmentUi* f : getUnsortedFragments()) {
        if (f->getFragmentName().split(" ").contains(name))
            return f;
    }
    return nullptr;
}

bool FragmentsController::jointFragment(FragmentUi *f1, const int piece1ID, FragmentUi *f2, const int piece2ID, const cv::Mat& originTransMat)
{
    const Piece p1 = f1->getPieces()[piece1ID];
    const Piece p2 = f2->getPieces()[piece2ID];
    const cv::Mat p1transMat = p1.transMat.clone();
    const cv::Mat p2transMat = p2.transMat.clone();
    cv::Mat p2transInv = p2.transMat.clone();
    cv::invert(p2transMat, p2transInv);

    cv::Mat src = Tool::QImageToMat(f1->getOriginalImage()); // 8UC4
    cv::Mat dst = Tool::QImageToMat(f2->getOriginalImage());

    PyObject* matArg = PyTuple_New(3);
    PyTuple_SetItem(matArg, 0, mat8UC42numpy(src));
    PyTuple_SetItem(matArg, 1, mat8UC42numpy(dst));
    Mat finalTransMat =  p1transMat * originTransMat * p2transInv;
    PyTuple_SetItem(matArg, 2, mat32FC12numpy(Tool::opencvToNormalTransMat(finalTransMat)));

    qInfo() << "run python FusionImage";
    auto locker = PyGILState_Ensure();
    PyObject* resObj = PyEval_CallObject(funcFusionImage, matArg);
    PyErr_Print();
    if (resObj == nullptr) {
        PyErr_Print();
        return false;
    }
    PyObject* resObjImg;
    PyObject* resObjTransMat;
    // PyArg_ParseTuple reference: https://docs.python.org/2.0/ext/parseTuple.html
    PyArg_ParseTuple(resObj, "O|O", &resObjImg, &resObjTransMat);
    Mat jointImg = pyObject2Mat(resObjImg, CV_8UC3);
    Mat offsetMat = pyObject2Mat(resObjTransMat, CV_32FC1);
    offsetMat = Tool::normalToOpencvTransMat(offsetMat);
    PyGILState_Release(locker);

    std::vector<Piece> pieces;
    for (Piece p : f1->getPieces()) {
        Piece newP = p;
        newP.transMat = p.transMat.clone();
        newP.transMat = offsetMat.clone() * newP.transMat;
        pieces.emplace_back(newP);
    }
    for (Piece p : f2->getPieces()) {
        Piece newP = p;
        newP.transMat = p.transMat.clone();
        newP.transMat = offsetMat.clone() * finalTransMat.clone() * newP.transMat;
        pieces.emplace_back(newP);
    }
    FragmentUi *newFragment = new FragmentUi(pieces, Tool::MatToQImage(Tool::Mat8UC3To8UC4(jointImg)), f1->getFragmentName() + " " + f2->getFragmentName());
    float newX = std::min(f1->scenePos().x(), f2->scenePos().x());
    float newY = std::min(f1->scenePos().y(), f2->scenePos().y());
    newFragment->setPos(newX, newY);
    newFragment->rotateAng = f1->rotateAng;
    newFragment->undoFragments.push_back(f1);
    newFragment->undoFragments.push_back(f2);
    std::vector<FragmentUi *> undoFragments;
    undoFragments.push_back(f1);
    undoFragments.push_back(f2);
    JointUndo *temp = new JointUndo(undoFragments, newFragment);
    CommonHeader::undoStack->push(temp);
    qInfo() << "joint fragments " << p1.pieceName << " and " << p2.pieceName;
    return true;
}

void FragmentsController::selectFragment()
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (FragmentUi *f : FragmentsController::getController()->getSortedFragments())
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
        for (FragmentUi *f : FragmentsController::getController()->getUnsortedFragments())
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

void FragmentsController::getGroundTruth(const QString& path)
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
    for (int i = 0; i < lines.length(); i += 2) {
        int fragID = lines[i].toInt();
        if (fragID - 1 != i / 2) {
            qCritical() << "error groundTruth";
        }
        QStringList nums = lines[i+1].replace("  ", " ").split(" ");
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

int FragmentsController::calcScore()
{
    int score = 0;
    for (FragmentUi* f : getUnsortedFragments()) {
        auto pieces = f->getPieces();
        for (int i = 1; i < (int)pieces.size(); ++i) {
            cv::Mat trans = Tool::getInvMat(pieces[0].transMat) * pieces[i].transMat;
            int p1 = pieces[0].pieceName.toInt();
            int p2 = pieces[i].pieceName.toInt();
            cv::Mat gt = Tool::getInvMat(groundTruth[p1]) * groundTruth[p2];
            float len = std::sqrt(std::pow(trans.at<float>(0, 2), 2) + std::pow(trans.at<float>(1, 2), 2));
            float len2 = std::sqrt(std::pow(gt.at<float>(0, 2), 2) + std::pow(gt.at<float>(1, 2), 2));
            float x = trans.at<float>(0, 2) / len / 2.0;
            float y = trans.at<float>(1, 2) / len / 2.0;
            float xx = gt.at<float>(0, 2) / len2 / 2.0;
            float yy = gt.at<float>(1, 2) / len2 / 2.0;
            float eachScore = 1.0 / 4 * std::pow(trans.at<float>(0, 0) / 2 - gt.at<float>(0, 0) / 2, 2) +
                              1.0 / 4 * std::pow(trans.at<float>(0, 1) / 2 - gt.at<float>(0, 1) / 2, 2) +
                    1.0 / 4 * std::pow(x - xx, 2) +
                    1.0 / 4 * std::pow(y - yy, 2);
            score += (1 - eachScore) * 100 + 0.5;
        }
    }
    return score;
}
