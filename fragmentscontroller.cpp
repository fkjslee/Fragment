#include "fragmentscontroller.h"
#include <Tool.h>
#include <QDir>
#include <commands.h>
#include <CommonHeader.h>
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")
#include <ctime>
#include <complex>
#include <numpy/arrayobject.h>
#define NO_IMPOET_ARRAY

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
}

FragmentsController *FragmentsController::controller = nullptr;
FragmentsController::FragmentsController()
{
}

void FragmentsController::createAllFragments(const QString &fragmentsPath)
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
        unsortedFragments.emplace_back(new FragmentUi(vec, QImage(dir.absolutePath() + "/" + fileName), QString("%1").arg(i)));
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
        for (Piece piece : splitFragment->getPieces())
        {
            piece.transMat = cv::Mat::eye(3, 3, CV_32FC1);
            std::vector<Piece> vec;
            vec.push_back(piece);
            FragmentUi *newSplitFragment = new FragmentUi(vec, QImage(piece.piecePath), piece.pieceName);
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

bool FragmentsController::jointFragment(FragmentUi *f1, const int piece1ID, FragmentUi *f2, const int piece2ID, const cv::Mat& transMat)
{
    Piece p1 = f1->getPieces()[piece1ID];
    Piece p2 = f2->getPieces()[piece2ID];
    cv::Mat p1Inv;
    cv::invert(p1.transMat, p1Inv);
    cv::Mat p2Inv;
    cv::invert(p1.transMat, p2Inv);
//    qInfo() << "p3 = ";
//    for (int i = 0; i < 3; ++i)
//        for (int j = 0; j < 3; ++j)
//            qInfo() << p2.transMat.at<float>(i, j);
//    qInfo() << "p4 = ";
//    for (int i = 0; i < 3; ++i)
//        for (int j = 0; j < 3; ++j)
//            qInfo() << p2Inv.at<float>(i, j);
    std::vector<Piece> pieces;
    for (Piece p : f1->getPieces()) {
        pieces.emplace_back(p);
    }
    for (Piece p : f2->getPieces()) {
        p.transMat = p.transMat * p2Inv;
        p.transMat = p.transMat * transMat;
        pieces.emplace_back(p);
    }
    static PyObject* pModule = PyImport_ImportModule("FusionImage");
    if (!pModule) {
        qCritical() << ("Cant open python file!\n");
        return 1;
    }

    static PyObject* pFunhello= PyObject_GetAttrString(pModule,"callFusionImage");
    if(!pFunhello){
        qCritical() << "Get function failed" << endl;
        return 0;
    }

    cv::Mat src = Tool::QImageToMat(f1->getOriginalImage()); // 8UC4
    cv::Mat dst = Tool::QImageToMat(f2->getOriginalImage());

    PyObject* matArg = PyTuple_New(3);
    PyTuple_SetItem(matArg, 0, mat8UC42numpy(src));
    PyTuple_SetItem(matArg, 1, mat8UC42numpy(dst));
    PyTuple_SetItem(matArg, 2, mat32FC12numpy(transMat));
//    qInfo() << "p2 = ";
//    for (int i = 0; i < 3; ++i)
//        for (int j = 0; j < 3; ++j)
//            qInfo() << p2Inv.at<float>(i, j);

    qInfo() << "run python FusionImage";
    auto locker = PyGILState_Ensure();
    PyObject* resObj = PyEval_CallObject(pFunhello, matArg);
    PyArrayObject* ret_array;
    PyArray_OutputConverter(resObj, &ret_array);
    npy_intp *shape = PyArray_SHAPE(ret_array);
    Mat resMat(shape[0], shape[1], CV_8UC3, PyArray_DATA(ret_array));
    PyGILState_Release(locker);

    FragmentUi *newFragment = new FragmentUi(pieces, Tool::MatToQImage(Tool::Mat8UC3To8UC4(resMat)), f1->getFragmentName() + " " + f2->getFragmentName());
    newFragment->setPos(QPoint((f1->scenePos().x() + f2->scenePos().x()) / 2, (f1->scenePos().y() + f2->scenePos().y()) / 2));
    newFragment->undoFragments.push_back(f1);
    newFragment->undoFragments.push_back(f2);
    std::vector<FragmentUi *> undoFragments;
    undoFragments.push_back(f1);
    undoFragments.push_back(f2);
    JointUndo *temp = new JointUndo(undoFragments, newFragment);
    CommonHeader::undoStack->push(temp);
    qInfo() << "joint fragments " << p1.pieceName << " and " << p2.pieceName;
//    qInfo() << "p2 = ";
//    for (Piece p : newFragment->getPieces()) {
//        qInfo() << p.pieceName;
//        for (int i = 0; i < 3; ++i)
//            for (int j = 0; j < 3; ++j)
//                qInfo() << p.transMat.at<float>(i, j);
//    }
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
