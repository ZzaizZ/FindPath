#include "cell.h"

Cell::Cell(double h, double w, int x = 0, int y = 0) :
    c_height(h),
    c_width(w)
{
    setPos(x, y);
}

QRectF Cell::boundingRect() const
{
    return QRectF(-c_width / 2, -c_height / 2, c_width, c_height);
}

EmptyCell::EmptyCell(double h, double w, int x = 0, int y = 0) :
    Cell(h, w, x, y)
{
    c_type = CellType::Empty;
}

void EmptyCell::paint(QPainter *paint, const QStyleOptionGraphicsItem*, QWidget*)
{
    paint->setPen(Qt::black);
    paint->setBrush(Qt::green);
    paint->drawRect(QRectF(-c_width/2, -c_height/2, c_width, c_height));
}

WallCell::WallCell(double h, double w, int x = 0, int y = 0) :
    Cell(h, w, x, y)
{
    c_type = CellType::Wall;
}

void WallCell::paint(QPainter *paint, const QStyleOptionGraphicsItem*, QWidget*)
{
    paint->setPen(Qt::black);
    paint->setBrush(Qt::gray);
    paint->drawRect(QRectF(-c_width/2, -c_height/2, c_width, c_height));
}

TextCell::TextCell(double h, double w, int x, int y, QString text) :
    PathCell(h, w, x, y)
{
    c_type = CellType::Text;
    this->text = text;
}

void TextCell::paint(QPainter *paint, const QStyleOptionGraphicsItem* p, QWidget* w)
{
    PathCell::paint(paint, p, w);
    paint->drawText(-c_width*0.15,c_height*0.25,text);
}

PathCell::PathCell(double h, double w, int x = 0, int y = 0) :
    Cell(h, w, x, y)
{
    c_type = CellType::Path;
}

void PathCell::paint(QPainter *paint, const QStyleOptionGraphicsItem*, QWidget*)
{
    paint->setPen(Qt::black);
    paint->setBrush(Qt::green);
    paint->drawRect(QRectF(-c_width/2, -c_height/2, c_width, c_height));
    paint->setPen(Qt::red);
    QPointF c_center(0, 0);
    paint->drawEllipse(c_center, c_width*0.3, c_height*0.3);
}
