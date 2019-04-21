#ifndef MAPFIELD_H
#define MAPFIELD_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <vector>
#include <queue>

#include "cell.h"

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

class Map : public QGraphicsScene
{
public:
    Map(int H, int W, QObject *parent);
    ~Map();
    std::vector<wall_coords_t> generateMap(int W, int H);    
    void FindTheWay(QPointF p_start, QPointF p_end);
private:
    std::vector<std::vector<Cell*>> map; // двумерный массив ячеек
    std::vector<wall_coords_t> walls; // массив координат со стенами
    std::vector<std::vector<int>> adj_matrix; // матрица инциденции для поля
    std::vector<int> path; // номера ячеек пути
    int m_w, m_h; // размеры поля в клетках
    // генерация матрицы инциденции для полученного поля
    std::vector<std::vector<int>> GetAdjMatrix(int W, int H, std::vector<wall_coords_t> walls);
    // перевод номера узла графа в координаты ячейки
    wall_coords_t NumberToCoord(int index, int W, int H);
    std::vector<int> BestPath(int n, int v_start, int v_end); // поиск в ширину
    Cell *m_start;
    Cell *m_end;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
};



#endif // MAPFIELD_H
