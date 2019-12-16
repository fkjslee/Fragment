#include "fragment.h"
#include <QTextStream>
#include <QtDebug>
#include <opencv2/opencv.hpp>
#include <QDir>
#include <Tool.h>
#include <algorithm>

using namespace cv;

Fragment* Fragment::draggingItem = nullptr;
std::vector<Fragment*> Fragment::unsortedFragments = std::vector<Fragment*>();
std::vector<Fragment*> Fragment::sortedFragments = std::vector<Fragment*>();

Fragment::Fragment(const QImage& image, const QString &fragmentName)
    : originalImage(image), fragmentName(fragmentName)
{
    this->showImage = image;
    setToolTip(fragmentName);
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
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
        unsortedFragments.emplace_back(new Fragment(QImage(dir.absolutePath() + "/" + fileName), QString("f%1").arg(++i)));
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
    unsortedFragments.erase(std::find(unsortedFragments.begin(), unsortedFragments.end(), f1));
    unsortedFragments.erase(std::find(unsortedFragments.begin(), unsortedFragments.end(), jointFragment.item));
    unsortedFragments.emplace_back(new Fragment(Tool::MatToQImage(jointMat), f1->fragmentName + " " + jointFragment.item->getFragmentName()));
    qInfo() << "joint fragmens " << f1->fragmentName << " and " << jointFragment.item->fragmentName << " with absGrayscale = " << jointFragment.absGrayscale;
    return true;
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
            const cv::Mat& m1 = Tool::QImage2Mat(item->getOriginalImage());
            const cv::Mat& m2 = Tool::QImage2Mat(otherFragment->getOriginalImage());
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
    return showImage.rect();
}

void Fragment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawImage(QPoint(0, 0), showImage);
}

void Fragment::scaledToWidth(int width)
{
    showImage = originalImage.scaledToWidth(width);
    qDebug() << "show 2 size = " << showImage.size() << "  " << width;
    update();
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
