#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QObject>
#include <vector>
#include <queue>
#include <QPoint>
#include <QTime>

#include "cell.h"

#define UNDEF -1 // обозначение ещё не посещённой вершины
#define CELL_SIZE 20

class PathFinder : public QObject
{

    Q_OBJECT

public:
    PathFinder(int w, int h);
public slots:
    void generateMap(int m_width, int m_height);
    void findTheWay(QPointF p_start, QPointF p_end);

private:
    int map_width, map_height;
    std::vector<QPoint> walls;
    std::vector<std::vector<int>> adj_matrix; // матрица инциденции для поля
    void getAdjMatrix();
    QPoint numberToCoord(int index);

signals:
    void signalAddCell(QPoint newCellCoord, CellType ct);
    void signalPathNotFound();
    void signalBuisyChanged(bool in_process);
};

#endif // PATHFINDER_H
