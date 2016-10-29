#ifndef CUSTOMVIEW_H
#define CUSTOMVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>
#include <sys/types.h>
#include "inductancecalc.h"
#include "looptype.h"

class CustomView : public QGraphicsView
{
public:
    CustomView(QWidget* parent = 0);

protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // CUSTOMVIEW_H
