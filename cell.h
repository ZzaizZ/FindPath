#ifndef CELL_H
#define CELL_H

#include <QWidget>
#include <QGraphicsItem>
#include <QPainter>

enum class CellType
{
    Empty,
    Wall,
    Start,
    End,
    Path
};

class Cell : public QGraphicsItem
{
public:
    Cell(double h, double w);
    CellType getType() { return c_type; }
protected:
    virtual QRectF boundingRect() const;
    double c_height, c_width;
    CellType c_type;
    QPoint pos_on_map;
};

class EmptyCell : public Cell
{
public:
    EmptyCell(double h, double w);
protected:
    virtual void paint(QPainter *paint, const QStyleOptionGraphicsItem*, QWidget*);
};

class WallCell : public Cell
{
public:
    WallCell(double h, double w);
protected:
    virtual void paint(QPainter *paint, const QStyleOptionGraphicsItem*, QWidget*);
};

class TextCell : public Cell
{
public:
    TextCell(double h, double w, QString text);
protected:
    QString text;
    virtual void paint(QPainter *paint, const QStyleOptionGraphicsItem*, QWidget*);
};

class PathCell : public Cell
{
public:
    PathCell(double h, double w);
protected:
    virtual void paint(QPainter *paint, const QStyleOptionGraphicsItem*, QWidget*);
};

#endif // CELL_H
