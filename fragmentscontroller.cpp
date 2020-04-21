#include "fragmentscontroller.h"
#include <Tool.h>
#include <QDir>
#include <commands.h>
#include <CommonHeader.h>
#include <Python.h>
#include <QJsonDocument>

namespace
{
    using namespace cv;
}

FragmentsController *FragmentsController::controller = nullptr;
FragmentsController::FragmentsController()
{
}

void FragmentsController::createAllFragments(const QString &fragmentsPath)
{
    Py_Initialize();
    if ( !Py_IsInitialized() )
    {
        qDebug() << "initialize failed!\n";
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
        vec.push_back(Piece(dir.absolutePath() + "/" + fileName));
        unsortedFragments.emplace_back(new FragmentUi(vec, QImage(dir.absolutePath() + "/" + fileName), QString("%1").arg(i)));
        ++i;
    }
    for (FragmentUi* fragment : getUnsortedFragments()) {
        qDebug() << fragment->getOriginalImage().format();
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

bool FragmentsController::splitSelectedFragments()
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
    CommonHeader::undoStack->push(new SplitUndo(undoFragments, redoFragments));
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

FragmentUi *FragmentsController::findFragmentByName(const QString &name)
{
    qDebug() << "all unsorted fragments";

    for (FragmentUi* f : getUnsortedFragments()) {
        qDebug() << f->getFragmentName();
    }
    for (FragmentUi* f : getUnsortedFragments()) {
        if (f->getFragmentName().split(" ").contains(name))
            return f;
    }
    return nullptr;
}

#ifdef MINE
bool FragmentsController::jointFragment(FragmentUi *f1, JointFragment jointFragment)
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
    JointUndo *temp = new JointUndo(undoFragments, newFragment);
    CommonHeader::undoStack->push(temp);
    qInfo() << "joint fragmens " << f1->getFragmentName() << " and " << f2->getFragmentName() << " with absGrayscale = " << jointFragment.absGrayscale;
    return true;
}
#else
bool FragmentsController::jointFragment(FragmentUi *f1, FragmentUi *f2, const QStringList& transformStr)
{
    qDebug() << "joint " << f1->getFragmentName() << "  " << f2->getFragmentName();
    static PyObject* pModule = PyImport_ImportModule("FusionImage");
    if (!pModule) {
        qDebug() << ("Cant open python file!\n");
        return -1;
    }

    static PyObject* pFunhello= PyObject_GetAttrString(pModule,"callFusionImage");
    if(!pFunhello){
        qDebug() << "Get function failed" << endl;
        return 0;
    }

    qDebug() << (f2 == nullptr);
    cv::Mat src = Tool::QImageToMat(f1->getOriginalImage()); // 8UC4
    cv::Mat dst = Tool::QImageToMat(f2->getOriginalImage());

    QJsonObject jsonSrc;
    jsonSrc["row"] = src.rows;
    jsonSrc["col"] = src.cols;
    jsonSrc["data"] = Tool::Mat8UC3ToString(src.type() == CV_8UC4 ? Tool::Mat8UC4To8UC3(src) : src);

    QJsonObject jsonDst;
    jsonDst["row"] = dst.rows;
    jsonDst["col"] = dst.cols;
    jsonDst["data"] = Tool::Mat8UC3ToString(src.type() == CV_8UC4 ? Tool::Mat8UC4To8UC3(dst) : src);

    QString trans;
    for (int i = 0; i < 9; ++i) {
        trans += transformStr[i];
        if (i != 8)
            trans += " ";
    }
//    QString str = "{\"col\":\"333\",\"data\":\"456x\",\"row\":\"111\"}";
//    QJsonDocument document = QJsonDocument::fromJson(str.toLocal8Bit().data());
//    QJsonObject jsonRes = document.object(); // str to json
//    QString s = QString(QJsonDocument(jsonSrc).toJson()); // json to str
//    s = s.replace("\n", "");
//    s = s.replace(" ", "");
//    qDebug() << QString(QJsonDocument(jsonRes).toJson());
    PyObject* args = PyTuple_New(3);
    PyTuple_SetItem(args, 0, Py_BuildValue("s", QString(QJsonDocument(jsonSrc).toJson()).toStdString().c_str()));
    PyTuple_SetItem(args, 1, Py_BuildValue("s", QString(QJsonDocument(jsonDst).toJson()).toStdString().c_str()));
    PyTuple_SetItem(args, 2, Py_BuildValue("s", trans.toStdString().c_str()));
    qInfo() << "run python FusionImage";
    auto gstate = PyGILState_Ensure();
    PyObject* resObj = PyEval_CallObject(pFunhello, args);
    PyGILState_Release(gstate);
//    PyObject* res = PyObject_CallFunction(pFunhello, "s", s.toStdString().c_str());

    QStringList res = QString(QLatin1String(PyBytes_AsString(resObj))).split(" ");
    int resRow = res[0].toInt();
    int resCol = res[1].toInt();
    cv::Mat resMat(resRow, resCol, CV_8UC3);
    int cnt = 2;
    for (int i = 0; i < resRow; ++i)
        for (int j = 0; j < resCol; ++j)
            for (int k = 0; k < 3; ++k) {
                resMat.at<Vec3b>(i, j)[k] = res[cnt++].toInt();
            }
//    cv::imwrite("fffu.png", resMat);
//    Py_Finalize();



    std::vector<Piece> pieces;
    for (Piece p : f1->getPiece())
        pieces.emplace_back(p);
    for (Piece p : f2->getPiece())
        pieces.emplace_back(p);
    FragmentUi *newFragment = new FragmentUi(pieces, Tool::MatToQImage(Tool::Mat8UC3To8UC4(resMat)), f1->getFragmentName() + " " + f2->getFragmentName());
    newFragment->setPos(QPoint((f1->scenePos().x() + f2->scenePos().x()) / 2, (f1->scenePos().y() + f2->scenePos().y()) / 2));
    newFragment->undoFragments.push_back(f1);
    newFragment->undoFragments.push_back(f2);
    std::vector<FragmentUi *> undoFragments;
    undoFragments.push_back(f1);
    undoFragments.push_back(f2);
    JointUndo *temp = new JointUndo(undoFragments, newFragment);
    CommonHeader::undoStack->push(temp);
    qInfo() << "joint fragmens " << f1->getFragmentName() << " and " << f2->getFragmentName();
    return true;
}
#endif

void FragmentsController::selectFragment()
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (FragmentUi *f : FragmentsController::getController()->getSortedFragments())
        {
            if (f->getSelected())
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
            if (f->getSelected())
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
