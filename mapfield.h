#ifndef MAPFIELD_H
#define MAPFIELD_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <vector>
#include <queue>

#define INF 99 // обозначение недостижимой вершины
#define UNDEF -1 // обозначение ещё не посещённой вершины
#define PATH_DOES_NOT_EXISTS -10 // единственное значение вектора пути, если цель недостижима

typedef int** grid_t;
// структрура координат расположения стены
typedef struct coords
{
    coords(int X,int Y)
    {
        x = X;
        y = Y;
    }
    int x;
    int y;
} wall_coords_t;

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

class Map : public QGraphicsScene
{
public:
    Map(int H, int W, QObject *parent);
    ~Map();
    std::vector<wall_coords_t> generateMap(int W, int H);    
    void FindTheWay(QPoint p_start, QPoint p_end);
private:
    std::vector<std::vector<Cell*>> map; // двумерный массив ячеек
    std::vector<wall_coords_t> walls; // массив координат со стенами
    std::vector<std::vector<int>> adj_matrix; // матрица инциденции для поля
    int m_w, m_h;
    bool searchPath();
    std::vector<std::vector<int>> GetAdjMatrix(int W, int H, std::vector<wall_coords_t> walls);
    wall_coords_t NumberToCoord(int index, int W, int H);
    std::vector<int> BestPath(int n, int v_start, int v_end);
    void DrawMap();
};



#endif // MAPFIELD_H
