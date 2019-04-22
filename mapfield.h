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

#define UNDEF -1 // обозначение ещё не посещённой вершины
#define CELL_SIZE 20

class Map : public QGraphicsScene
{
public:
    Map(int H, int W, QObject *parent);
    ~Map();
    void generateMap(int W, int H);
    void FindTheWay(QPointF p_start, QPointF p_end);
private:
    std::vector<std::vector<Cell*>> map; // двумерный массив ячеек поля (для отрисовки)
    std::vector<QPoint> walls; // массив координат со стенами
    std::vector<std::vector<int>> adj_matrix; // матрица инциденции для поля
    std::vector<QPoint> path; // номера ячеек пути
    std::vector<Cell*> path_cell; // ячейки пути
    int m_w, m_h; // размеры поля в клетках
    // генерация матрицы инциденции для полученного поля
    std::vector<std::vector<int>> GetAdjMatrix(int W, int H, std::vector<QPoint> walls);
    // перевод номера узла графа в координаты ячейки
    QPoint NumberToCoord(int index, int W, int H);
    std::vector<QPoint> BestPath(int n, int v_start, int v_end); // поиск в ширину
    void ClearPath(); // очистка вектора ячеек пути
    Cell *m_start;
    Cell *m_end;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
};



#endif // MAPFIELD_H
