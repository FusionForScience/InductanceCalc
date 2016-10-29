#include "inductancecalc.h"
#include "ui_inductancecalc.h"

#include <QDebug>

inductanceCalc::inductanceCalc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::inductanceCalc)
{
    // BASIC INITIALIZATION

    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    ui->geometryView->setScene(scene);
    ui->geometryView->setDragMode(QGraphicsView::NoDrag);

    loops = new LoopType[5];
    loopType = 0;

    pen.setJoinStyle(Qt::MiterJoin);

    Tri = NULL;



    // RESTRICT INPUT

    QRegularExpression loopReg("\\d+(?=\\s*w:)");
    QRegularExpression doubleNum("\\d+\\.\\d+(?=\\s*w:)");

    QValidator *loopVal = new QRegularExpressionValidator(loopReg , this);
    QValidator *doubleNumVal = new QRegularExpressionValidator(doubleNum , this);


    QDoubleValidator *relPermVal = new QDoubleValidator(this);
    relPermVal->setBottom(0);
    relPermVal->setTop(1.0);
    relPermVal->setNotation(QDoubleValidator::StandardNotation);

    ui->numTurnsEditor->setValidator(loopVal);
    ui->relPermEditor->setValidator(relPermVal);
    ui->wireRadiusEditor->setValidator(doubleNumVal);
    ui->heightEditor->setValidator(doubleNumVal);
    ui->widthEditor->setValidator(doubleNumVal);
}

inductanceCalc::~inductanceCalc()
{
    delete ui;
}

double inductanceCalc::calculate(int type)
{
    const double PI = 3.14159265358979323846;
    const double PERMCONST = 4 * PI * pow(10.0 , -7);

    qDebug() << PERMCONST;

    double result;

    double N;
    double W;
    double H;
    double R;
    double MR;

    N = loops[loopType].numTurns;
    W = loops[loopType].width;
    H = loops[loopType].height;
    R = loops[loopType].wireRadius;
    MR = loops[loopType].relPerm;

    switch(loopType)
    {
        case 0:
            result = -2 * (W + H);
            result += 2 * pow(pow(H , 2) + pow(W , 2) , .5);
            result -= H * log((H + pow(pow(H , 2) + pow(W , 2) , 0.5)) / W);
            result -= W * log((W + pow(pow(H , 2) + pow(W , 2) , 0.5)) / H);
            result += H * log(2 * H / R);
            result += W * log(2 * W / R);
            result *= PERMCONST * MR / PI * pow(N , 2);
            break;

        case 1:
        case 2:
        case 3:
        case 4:

        default:
            result = loops[type].height + loops[type].numTurns + loops[type].relPerm + loops[type].width + loops[type].wireRadius;
    }
    return result;
}

void inductanceCalc::on_loopCombo_currentIndexChanged(int index)
{
    loopType = index;

    ui->numTurnsEditor->setText(QString::number(loops[loopType].numTurns));
    ui->wireRadiusEditor->setText(QString::number(loops[loopType].wireRadius));
    ui->relPermEditor->setText(QString::number(loops[loopType].relPerm));
    ui->heightEditor->setText(QString::number(loops[loopType].height));
    ui->widthEditor->setText(QString::number(loops[loopType].width));

    if(loops[loopType].numTurns == 0)
        ui->numTurnsEditor->setText("");

    if(loops[loopType].wireRadius == 0)
        ui->wireRadiusEditor->setText("");

    if(loops[loopType].relPerm == 0)
        ui->relPermEditor->setText("");

    if(loops[loopType].height == 0)
        ui->heightEditor->setText("");

    if(loops[loopType].width == 0)
        ui->widthEditor->setText("");

    drawGeometry();
}



// LINE EDITOR SLOTS

void inductanceCalc::on_numTurnsEditor_textChanged()
{
    if(ui->numTurnsEditor->text() == "")
        loops[loopType].numTurns = 0;
    else
        loops[loopType].numTurns = (ui->numTurnsEditor->text()).toInt();

    drawGeometry();
}

void inductanceCalc::on_wireRadiusEditor_textChanged()
{
    if(ui->wireRadiusEditor->text() == "")
        loops[loopType].wireRadius = 0;
    else
        loops[loopType].wireRadius = (ui->wireRadiusEditor->text()).toInt();

    if(loops[loopType].wireRadius >= ui->geometryView->height() || loops[loopType].wireRadius >= ui->geometryView->width())
        loops[loopType].scale = scale(10);

    drawGeometry();
}

void inductanceCalc::on_relPermEditor_textChanged()
{
    double number;

    if(ui->relPermEditor->text() == "")
        loops[loopType].relPermLastString = "";
    else
    {
        number = (ui->relPermEditor->text()).toDouble();

        if(number >= 0 && number < 1)
        {
            loops[loopType].relPerm = number;
            loops[loopType].relPermLastString.setNum(number);
        }
        else if (number == 1)
        {
            ui->relPermEditor->setText("1");
        }
        else
            ui->relPermEditor->setText(loops[loopType].relPermLastString);

        drawGeometry();
    }
}

void inductanceCalc::on_heightEditor_textChanged()
{
    if(ui->heightEditor->text() == "")
        loops[loopType].height = 0;
    else
        loops[loopType].height = (ui->heightEditor->text()).toInt();

    switch(loopType)
    {
        case 1:
            loops[1].width = (ui->heightEditor->text()).toInt();
            break;

        case 2:
            loops[2].width = (ui->heightEditor->text()).toInt();
            break;

        case 3:
            loops[3].width = (ui->heightEditor->text()).toInt();
            break;

        default:
            break;
    }

    drawGeometry();
}

void inductanceCalc::on_widthEditor_textChanged()
{
    double temp;

    if(ui->widthEditor->text() == "")
        loops[loopType].width = 0;
    else
        loops[loopType].width = (ui->widthEditor->text()).toInt();

    temp = loops[loopType].width;

    if(loopType == 4 && loops[loopType].width >= 2 * loops[loopType].height)
    {
        QMessageBox::warning(this , tr("Warning") , tr("One side of a triangle must be shorter than the sum of the other two."));
        loops[loopType].width = temp;
        ui->widthEditor->setText(QString::number(temp));
    }




    drawGeometry();
}

void inductanceCalc::on_autoScaleCheck_toggled(bool checked)
{
    autoScaleOn = ui->autoScaleCheck->isChecked();

    loops[loopType].scale = scale(75);

    if(checked)
    {
        drawGeometry();
    }
}

void inductanceCalc::on_calculateButton_clicked()
{
    ui->answerLabel->setText(QString::number(calculate(loopType)));
}

void inductanceCalc::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.15;

    if(!autoScaleOn)
    {
        if(event->delta() > 0)
        {
            loops[loopType].scale *= scaleFactor;

            if(loops[loopType].scale * loops[loopType].width > ui->geometryView->width() || loops[loopType].scale * loops[loopType].height > ui->geometryView->height())
                loops[loopType].scale = scale(20);
        }
        else
            loops[loopType].scale /= scaleFactor;

        event->accept();

        drawGeometry();
    }
}


// PRIVATE CONVENIENCE FUNCTIONS

double inductanceCalc::scale(int buffer)
{
    double height;
    double width;

    double heightRatio;
    double widthRatio;

    height = loops[loopType].height;
    width = loops[loopType].width;

    height += loops[loopType].wireRadius;
    width += loops[loopType].wireRadius;

    if(height == 0)
        height = 1;
    if(width == 0)
        width = 1;

    height += loops[loopType].wireRadius / 2;
    width += loops[loopType].wireRadius / 2;

    int viewHeight;
    int viewWidth;

    viewHeight = ui->geometryView->height();
    viewWidth = ui->geometryView->width();

    viewHeight -= buffer;
    viewWidth -= buffer;

    heightRatio = viewHeight / height;
    widthRatio = viewWidth / width;

    return heightRatio < widthRatio ? heightRatio : widthRatio;
}

void inductanceCalc::drawGeometry()
{
    double height;
    double width;

    height = loops[loopType].height;
    width = loops[loopType].width;

    pen.setWidth(loops[loopType].wireRadius * loops[loopType].scale);

    if(autoScaleOn)
        on_autoScaleCheck_toggled(false);

    scene->clear();
    ui->geometryView->update();



    switch(loopType)
    {
        case 0:
            toggleWidthVisible(true);
            ui->heightLabel->setText("Height: ");
            ui->widthLabel->setText("Width: ");

            scene->clear();
            rectangle = scene->addRect(0 - loops[loopType].scale * loops[loopType].width / 2 , 0 - loops[loopType].scale * loops[loopType].height / 2 , loops[loopType].scale * loops[loopType].width , loops[loopType].scale * loops[loopType].height , pen);
        break;

        case 1:
            toggleWidthVisible(false);
            ui->heightLabel->setText("Side width: ");

            scene->clear();
            rectangle = scene->addRect(0 - loops[loopType].scale * loops[loopType].height / 2 , 0 - loops[loopType].scale * loops[loopType].height / 2 , loops[loopType].scale * loops[loopType].height , loops[loopType].scale * loops[loopType].height , pen);
        break;

        case 2:
            toggleWidthVisible(false);
            ui->heightLabel->setText("Radius: ");

            scene->clear();
            ellipse = scene->addEllipse(0 - loops[loopType].scale * loops[loopType].height / 2 , 0 - loops[loopType].scale * loops[loopType].height / 2 , loops[loopType].scale * loops[loopType].height , loops[loopType].scale * loops[loopType].height , pen);
        break;

        case 3:
            toggleWidthVisible(false);
            ui->heightLabel->setText("Side width: ");

            scene->clear();

            triangle.append(QPoint(((int)(0 - loops[loopType].scale * loops[loopType].height / 2.0)) , ((int)(loops[loopType].scale * loops[loopType].height * sqrt(3) / 4))));
            triangle.append(QPoint(((int)(loops[loopType].scale * loops[loopType].height / 2.0)) , ((int)(loops[loopType].scale * loops[loopType].height * sqrt(3) / 4))));
            triangle.append(QPoint(0 , ((int)(0 - loops[loopType].scale * loops[loopType].height * sqrt(3) / 4))));
            triangle.append(QPoint(((int)(0 - loops[loopType].scale * loops[loopType].height / 2.0)) , ((int)(loops[loopType].scale * loops[loopType].height * sqrt(3) / 4))));

            Tri = scene->addPolygon(triangle , pen);

        break;

        case 4:
            toggleWidthVisible(true);
            ui->heightLabel->setText("Width of Congruent Sides: ");
            ui->widthLabel->setText("Width of Base: ");

            scene->clear();

            if(width == 0)
                width = 1;
            if(height == 0)
                height = 1;

            double triangleHeight;


            triangleHeight = sqrt(pow(loops[loopType].scale * height , 2) - pow((loops[loopType].scale * width / 2) , 2));

            triangle.append(QPoint((int)(-loops[loopType].scale * width / 2) , (int)(triangleHeight / 2)));
            triangle.append(QPoint((int)(loops[loopType].scale * width / 2) , (int)(triangleHeight / 2)));
            triangle.append(QPoint(0 , (int)(-triangleHeight / 2)));
            triangle.append(QPoint((int)(-loops[loopType].scale * width / 2) , (int)(triangleHeight / 2)));

            Tri = scene->addPolygon(triangle , pen);

        break;
    }

    triangle.clear();
    //delete Tri;
    //Tri = NULL;
}

void inductanceCalc::toggleWidthVisible(bool toggle)
{
    ui->widthLabel->setVisible(toggle);
    ui->widthEditor->setVisible(toggle);
    ui->widthUnit->setVisible(toggle);
}

