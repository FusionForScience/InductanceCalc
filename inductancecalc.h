#ifndef INDUCTANCECALC_H
#define INDUCTANCECALC_H

#include <QtMath>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtGui>
#include <QWheelEvent>
#include <QRegExp>
#include <QRegularExpression>
#include <QDoubleValidator>
#include <QWheelEvent>
#include <QMessageBox>

#include "customview.h"
#include "looptype.h"

namespace Ui {
class inductanceCalc;
}

class inductanceCalc : public QWidget
{
    Q_OBJECT

    friend class CustomView;

public:
    explicit inductanceCalc(QWidget *parent = 0);
    ~inductanceCalc();

protected:
    //virtual void wheelEvent(QWheelEvent *event);

private slots:
    double calculate(int type);

    //void on_loopCombo_activated();

    void on_numTurnsEditor_textChanged();

    void on_wireRadiusEditor_textChanged();

    void on_relPermEditor_textChanged();

    void on_heightEditor_textChanged();

    void on_widthEditor_textChanged();

    void on_autoScaleCheck_toggled(bool checked);

    void on_calculateButton_clicked();

    void wheelEvent(QWheelEvent *event);

    void on_loopCombo_currentIndexChanged(int index);

private:
    Ui::inductanceCalc *ui;

    QPolygon triangle;

    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangle;
    QGraphicsPolygonItem *Tri;
    QPen pen;

    LoopType *loops;

    QMessageBox msgBox;

    int loopType;

    double previousScale;

    bool autoScaleOn = false;

    double scale(int buffer);

    void drawGeometry();

    void toggleWidthVisible(bool toggle);
};

#endif // INDUCTANCECALC_H
