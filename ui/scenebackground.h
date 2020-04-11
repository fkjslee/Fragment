#ifndef SCENEBACKGROUND_H
#define SCENEBACKGROUND_H

#include<QImage>


class SceneBackground : public QImage
{
public:
    SceneBackground();
    void setScale(QPoint point);
    void setScale(int width, int height);
    void updateBackground();

private:
    float leftDivRight = 3;
    float upDivDown = 3;
};

#endif // SCENEBACKGROUND_H
