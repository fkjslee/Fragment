#include "coloritem.h"
#include <iostream>
#include <QTextStream>
#include <QtDebug>
#include <opencv2/opencv.hpp>
#include <QDir>
#include <Tool.h>

using namespace cv;

Fragment* Fragment::draggingItem = nullptr;
std::set<Fragment*> Fragment::unsortedFragments = std::set<Fragment*>();
std::set<Fragment*> Fragment::sortedFragments = std::set<Fragment*>();

namespace  {
}

Fragment::Fragment(const QImage& image, const QString &fragmentName)
{
    this->fragmentName = fragmentName;
    this->image = image;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void Fragment::createFragments()
{
    qInfo() << "createFragments";
    QDir dir("./fragment/");
    QStringList filter;
    filter << "*.jpg" << "*.png";
    QStringList nameList = dir.entryList(filter);
    int i = 0;
    for (const QString& fileName : nameList) {
        unsortedFragments.insert(new Fragment(QImage(dir.absolutePath() + "/" + fileName), QString("f%1").arg(++i)));
    }
}

bool Fragment::sortFragment(Fragment *frag)
{
    if(unsortedFragments.find(frag) == unsortedFragments.end())
        return false;
    unsortedFragments.erase(frag);
    sortedFragments.insert(frag);
    return true;
}

bool Fragment::unsortFragment(Fragment *frag)
{
    if(sortedFragments.find(frag) == sortedFragments.end())
        return false;
    sortedFragments.erase(frag);
    unsortedFragments.insert(frag);
    return true;
}

bool Fragment::jointFragment(Fragment *f1, JointFragment jointFragment)
{
    const cv::Mat& m1 = Tool::QImage2Mat(f1->image);
    const cv::Mat& m2 = Tool::QImage2Mat(jointFragment.item->image);
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
    unsortedFragments.erase(unsortedFragments.find(f1));
    unsortedFragments.erase(unsortedFragments.find(jointFragment.item));
    unsortedFragments.insert(new Fragment(Tool::MatToQImage(jointMat), f1->fragmentName + " " + jointFragment.item->getFragmentName()));
    return true;
}

std::set<Fragment *> Fragment::getSortedFragments()
{
    return sortedFragments;
}

std::set<Fragment *> Fragment::getUnsortedFragments()
{
    return unsortedFragments;
}

std::vector<JointFragment> Fragment::getMostPossibleColorItems(Fragment *item)
{
    std::vector<JointFragment> res;
    if (item == nullptr) {
        std::set<Fragment*> unsorted_fragments = Fragment::getUnsortedFragments();
        for (Fragment* unsorted_fragment : unsorted_fragments) {
            res.emplace_back(JointFragment(unsorted_fragment, JointMethod::leftRight, 0));
            if (res.size() >= 5)
                break;
        }
    } else {
        JointFragment minFragment(nullptr, JointMethod::leftRight, 0x3f3f3f3f);
        for (Fragment* otherFragment : Fragment::getUnsortedFragments()) {
            if (item == otherFragment) continue;
            const cv::Mat& m1 = Tool::QImage2Mat(item->getImage());
            const cv::Mat& m2 = Tool::QImage2Mat(otherFragment->getImage());
            double absGrayscale;
            absGrayscale = Tool::calcLeftRightAbsGrayscale(m1, m2);
            if (absGrayscale < minFragment.absGrayscale)
                minFragment = JointFragment(otherFragment, JointMethod::leftRight, absGrayscale);

            absGrayscale = Tool::calcLeftRightAbsGrayscale(m2, m1);
            if (absGrayscale < minFragment.absGrayscale)
                minFragment = JointFragment(otherFragment, JointMethod::rightLeft, absGrayscale);

            absGrayscale = Tool::calcUpDownAbsGrayscale(m1, m2);
            if (absGrayscale < minFragment.absGrayscale)
                minFragment = JointFragment(otherFragment, JointMethod::upDown, absGrayscale);

            absGrayscale = Tool::calcUpDownAbsGrayscale(m2, m1);
            if (absGrayscale < minFragment.absGrayscale)
                minFragment = JointFragment(otherFragment, JointMethod::downUp, absGrayscale);
        }

        if (minFragment.item != nullptr)
            res.emplace_back(minFragment);
    }
    return res;
}

QRectF Fragment::boundingRect() const
{
    return image.rect();
}

void Fragment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawImage(QPoint(0, 0), image);
}

void Fragment::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    draggingItem = this;
    setCursor(Qt::ClosedHandCursor);
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

void Fragment::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept();
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
    mime->setColorData(color);
    QPixmap pixmap(image.width(), image.height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
//    painter.translate(15, 15);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter, nullptr, nullptr);
    painter.end();

    pixmap.setMask(pixmap.createHeuristicMask());

    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(15, 20));


    drag->exec();
    setCursor(Qt::OpenHandCursor);
}
