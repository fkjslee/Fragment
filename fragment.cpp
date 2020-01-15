#include "fragment.h"
#include <QTextStream>
#include <QtDebug>
#include <opencv2/opencv.hpp>
#include <QDir>
#include <Tool.h>
#include <algorithm>
#include <QStyleOptionGraphicsItem>
#include <QMetaEnum>
#include <Tool.h>

using namespace cv;

Fragment::Fragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName)
    : FragmentUi(pieces, originalImage, fragmentName)
{
}
