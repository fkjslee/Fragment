#include "fragment.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

std::set<Fragment*> Fragment::unsortedFragments = std::set<Fragment*>();
std::set<Fragment*> Fragment::sortedFragments = std::set<Fragment*>();

namespace  {

cv::Mat QImage2Mat(QImage image)
{
    cv::Mat mat;
    //qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
//        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
//        cv::cvtColor(mat, mat, COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default: ;
    }
    return mat;
}


QImage MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

int calcLeftRightAbsGrayscale(const cv::Mat& left, const cv::Mat& rhs) {
    int res = 0;
    if (left.type() != rhs.type() || left.size() != rhs.size() || left.type() != CV_8UC4)
        return 0x3f3f3f3f;
    int c = left.cols - 1;
    for (int r = 0; r < left.rows; ++r) {
        for (int channel = 0; channel < 4; ++channel)
            res += abs(left.at<Vec4b>(r, c)[0] - rhs.at<Vec4b>(r, 0)[channel]);
    }
    return res;
}

}

Fragment::Fragment(const QImage& image, const QString& fragmentName)
    : QObject(), image(image), fragmentName(fragmentName)
{
    qDebug() << (this->image.size().width()) << " " << this->image.size().height();
    
    property = QColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
}

Fragment::Fragment(const Fragment &rhs)
{
    Q_UNUSED(rhs)
}

Fragment::~Fragment()
{
}

void Fragment::createFragments()
{
    qDebug() << "createFragments";
    QDir dir("./fragment/");
    QStringList filter;
    filter << "*.jpg" << "*.png";
    QStringList nameList = dir.entryList(filter);
    for (const QString& fileName : nameList) {
        unsortedFragments.insert(new Fragment(QImage(dir.absolutePath() + "/" + fileName), fileName));
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

std::set<Fragment *> Fragment::getSortedFragments()
{
    return sortedFragments;
}

std::set<Fragment *> Fragment::getUnsortedFragments()
{
    return unsortedFragments;
}

std::vector<JointFragment> Fragment::getMostPossibleFragments(Fragment *f)
{
    std::vector<JointFragment> res;
    if (f == nullptr) {
        std::set<Fragment*> unsorted_fragments = getUnsortedFragments();
        for (Fragment* unsorted_fragment : unsorted_fragments) {
            res.emplace_back(JointFragment(unsorted_fragment, JointMethod::leftRight, 0));
            if (res.size() >= 5)
                break;
        }
    } else {
        JointFragment minFragment(nullptr, JointMethod::leftRight, 0x3f3f3f3f);
        for (Fragment* otherFragment : getUnsortedFragments()) {
            if (f == otherFragment) continue;
            int absGrayscale;
            absGrayscale = calcLeftRightAbsGrayscale(QImage2Mat(f->getImage()), QImage2Mat(otherFragment->getImage()));
            if (absGrayscale < minFragment.absGrayscale)
                minFragment = JointFragment(otherFragment, JointMethod::leftRight, absGrayscale);

            absGrayscale = calcLeftRightAbsGrayscale(QImage2Mat(otherFragment->getImage()), QImage2Mat(f->getImage()));
            if (absGrayscale < minFragment.absGrayscale)
                minFragment = JointFragment(otherFragment, JointMethod::rightLeft, absGrayscale);
            absGrayscale = calcLeftRightAbsGrayscale(QImage2Mat(otherFragment->getImage()), QImage2Mat(f->getImage()));
        }
        res.emplace_back(minFragment);
    }
    return res;
}

void Fragment::receiveColorItemDragging(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "receiveColorItemDragging";
}

