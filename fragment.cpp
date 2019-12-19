#include "fragment.h"
#include <QTextStream>
#include <QtDebug>
#include <opencv2/opencv.hpp>
#include <QDir>
#include <Tool.h>
#include <algorithm>
#include <mainwindow.h>
#include <QStyleOptionGraphicsItem>
#include <QMetaEnum>

using namespace cv;

Fragment* Fragment::draggingItem = nullptr;
std::vector<Fragment*> Fragment::unsortedFragments = std::vector<Fragment*>();
std::vector<Fragment*> Fragment::sortedFragments = std::vector<Fragment*>();
std::vector<Fragment*> Fragment::chosenFragments = std::vector<Fragment*>();

JointFragment Fragment::mostPossibleJointMethod(Fragment* f1, Fragment* f2) {
    JointFragment minFragment(nullptr, JointMethod::leftRight, 0x3f3f3f3f);
    const cv::Mat& m1 = Tool::QImage2Mat(f1->getOriginalImage());
    const cv::Mat& m2 = Tool::QImage2Mat(f2->getOriginalImage());
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

Fragment::Fragment(const std::vector<Piece>& pieces, const QImage& originalImage, const QString &fragmentName)
    : pieces(pieces), originalImage(originalImage), fragmentName(fragmentName)
{
    this->showImage = originalImage;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setFlag(QGraphicsItem::ItemIsMovable, true);
}

void Fragment::createAllFragments(const QString& fragmentsPath)
{
    qInfo() << "createAllFragments";
    QDir dir(fragmentsPath);
    QStringList filter;
    filter << "*.jpg" << "*.png";
    QStringList nameList = dir.entryList(filter);
    int i = 0;
    for (const QString& fileName : nameList) {
        std::vector<Piece> vec;
        vec.push_back(Piece(dir.absolutePath() + "/" + fileName, QString("f%1").arg(++i)));
        unsortedFragments.emplace_back(new Fragment(vec, QImage(dir.absolutePath() + "/" + fileName), QString("f%1").arg(i)));
    }
}

bool Fragment::sortFragment(Fragment *frag)
{
//    if(unsortedFragments.find(frag) == unsortedFragments.end())
//        return false;
//    unsortedFragments.erase(frag);
//    sortedFragments.insert(frag);
    return true;
}

bool Fragment::unsortFragment(Fragment *frag)
{
//    if(sortedFragments.find(frag) == sortedFragments.end())
//        return false;
//    sortedFragments.erase(frag);
//    unsortedFragments.insert(frag);
    return true;
}

bool Fragment::jointFragment(Fragment *f1, JointFragment jointFragment)
{
    const cv::Mat& m1 = Tool::QImage2Mat(f1->getOriginalImage());
    const cv::Mat& m2 = Tool::QImage2Mat(jointFragment.item->getOriginalImage());
    cv::Mat jointMat;
    switch(jointFragment.method) {
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
    Fragment* newFragment = new Fragment(pieces, Tool::MatToQImage(jointMat), f1->fragmentName + " " + jointFragment.item->getFragmentName());
    newFragment->setPos(f1->scenePos());
    unsortedFragments.emplace_back(newFragment);
    qInfo() << "joint fragmens " << f1->fragmentName << " and " << jointFragment.item->fragmentName << " with absGrayscale = " << jointFragment.absGrayscale;
    return true;
}

bool Fragment::splitSelectedFragments()
{
    for (Fragment* splitFragment : getSelectedFragments()) {
        Tool::eraseInVector(unsortedFragments, splitFragment);
        for (Piece piece : splitFragment->getPiece()) {
            std::vector<Piece> vec;
            vec.push_back(piece);
            unsortedFragments.emplace_back(new Fragment(vec, QImage(piece.piecePath), piece.pieceName));
        }
    }
    return true;
}

void Fragment::reverseChosenFragment(Fragment *f)
{
    std::vector<Fragment*>::iterator iterF = std::find(chosenFragments.begin(), chosenFragments.end(), f);
    if (iterF == chosenFragments.end())
        chosenFragments.emplace_back(f);
    else
        chosenFragments.erase(iterF);
}

const std::vector<Fragment *> Fragment::getSelectedFragments()
{
    std::vector<Fragment*> selectedFragments;
    for (Fragment* f : getUnsortedFragments()) {
        if (f->selected)
            selectedFragments.emplace_back(f);
    }
    return selectedFragments;
}

void Fragment::imageSizeChanged(const int value)
{
    qDebug() << "change value = " << value;
}

std::vector<Fragment *> Fragment::getSortedFragments()
{
    return sortedFragments;
}

std::vector<Fragment *> Fragment::getUnsortedFragments()
{
    return unsortedFragments;
}

std::vector<JointFragment> Fragment::getMostPossibleFragments(Fragment *item)
{
    std::vector<JointFragment> res;
    if (item == nullptr) {
        std::vector<Fragment*> unsorted_fragments = Fragment::getUnsortedFragments();
        for (Fragment* unsorted_fragment : unsorted_fragments) {
            res.emplace_back(JointFragment(unsorted_fragment, JointMethod::leftRight, 0));
            if (res.size() >= 5)
                break;
        }
    } else {
        JointFragment minFragment(nullptr, JointMethod::leftRight, 0x3f3f3f3f);
        for (Fragment* otherFragment : Fragment::getUnsortedFragments()) {
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

QRectF Fragment::boundingRect() const
{
    return showImage.rect();
}

void Fragment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawImage(QPoint(0, 0), showImage);
}

void Fragment::scaledToWidth(const double scale)
{
    this->scale = scale;
    update();
}

void Fragment::reverseSelectState()
{
    selected = !selected;
    update();
}

void Fragment::update(const QRectF &rect)
{
    qDebug() << "selected " << selected;
    showImage = originalImage.scaledToWidth(int(originalImage.width() * scale));
    int alpha;
    if (selected) alpha = 100;
    else alpha = 255;
    QPainter painter(&showImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(0, 0, QPixmap::fromImage(showImage));
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.fillRect(showImage.rect(), QColor(0, 0, 0, alpha));
    painter.end();
    QGraphicsItem::update(rect);
}

void Fragment::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    qDebug() << "mousePressEvent";
    if (MainWindow::getCtlStatus()) {
        selected = !selected;
        update();
    } else {
        draggingItem = this;
        setCursor(Qt::ClosedHandCursor);
    }
}

void Fragment::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    draggingItem = nullptr;
    setCursor(Qt::OpenHandCursor);
}

void Fragment::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClickItem(this);
    Q_UNUSED(event)
}

void Fragment::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event)
}

void Fragment::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug() << "fragment dragEnterEvent";
    event->accept();
}

void Fragment::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        return;
    }
    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    drag->setMimeData(mime);
    QPixmap pixmap(showImage.width(), showImage.height());
    pixmap.fill(Qt::white);
    qDebug() << "showImage.size = " << showImage.size();

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, nullptr, nullptr);
    painter.end();

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    biasPos = event->scenePos() - this->scenePos();
    drag->setHotSpot(biasPos.toPoint());

    drag->exec();
    setCursor(Qt::OpenHandCursor);
}
