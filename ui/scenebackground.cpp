#include "scenebackground.h"
#include <QtDebug>

SceneBackground::SceneBackground() : QImage(100, 200, QImage::Format_RGB32)
{
}

void SceneBackground::setScale(QPoint point)
{
    setScale(point.x(), point.y());
}

void SceneBackground::setScale(int width, int height)
{
    QImage temp = this->scaled(width, height);
    this->swap(temp);
    updateBackground();
}

void SceneBackground::updateBackground()
{
    const int width = this->width();
    const int height = this->height();
    const int horizentalCenter = int(leftDivRight / (leftDivRight + 1) * width);
    const int verticalCenter = int(upDivDown / (upDivDown + 1) * height);
    qDebug() << "center = " << horizentalCenter << " width = " << width;
    for (int i = 0; i < horizentalCenter; ++i)
        for (int j = 0; j < verticalCenter; ++j)
            setPixel(i, j, qRgb(255, 0, 0));

    for (int i = 0; i < horizentalCenter; ++i)
        for (int j = verticalCenter; j < height; ++j)
            setPixel(i, j, qRgb(0, 255, 0));

    for (int i = horizentalCenter; i < width; ++i)
        for (int j = 0; j < height; ++j)
            setPixel(i, j, qRgb(0, 0, 255));
}
