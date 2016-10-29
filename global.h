#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <QGraphicsView>

class Global : public QWidget , public QGraphicsView
{
    Q_OBJECT
public:
    explicit Global(QWidget *parent = 0);

signals:

public slots:
};

#endif // GLOBAL_H
