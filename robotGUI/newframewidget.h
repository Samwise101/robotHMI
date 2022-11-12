#ifndef NEWFRAMEWIDGET_H
#define NEWFRAMEWIDGET_H

#include <QWidget>
#include <QFrame>

class QFrame;
class QPaintEvent;

class NewFrameWidget : public QWidget

{
    Q_OBJECT

public:
    NewFrameWidget(QWidget *parent = 0);
    ~NewFrameWidget();
    void draw(QPainter* painter, QPaintEvent event);

protected:
    void paintEvent(QPaintEvent* event);

private:
    double offset;

public:

};

#endif // NEWFRAMEWIDGET_H
