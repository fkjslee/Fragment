#ifndef ROBOTPART_H
#define ROBOTPART_H

#include <QMainWindow>
#include <QGraphicsObject>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class RobotPart : public QGraphicsObject
{
public:
  RobotPart(QGraphicsItem *parent = nullptr);

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;

  QColor color;
  bool dragOver;
};
#endif // ROBOTPART_H
