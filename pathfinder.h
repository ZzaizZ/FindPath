#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QObject>
#include <vector>
#include <queue>
#include <QPoint>

#define UNDEF -1 // обозначение ещё не посещённой вершины
#define CELL_SIZE 20

class PathFinder : QObject
{

    Q_OBJECT

public:
    PathFinder(int w, int h, std::vector<QPoint> walls);
public slots:
    std::vector<QPoint> findTheWay(QPointF p_start, QPointF p_end);
private:
    int W, H;
    std::vector<QPoint> walls;
    std::vector<std::vector<int>> adj_matrix; // матрица инциденции для поля
    void GetAdjMatrix();
    QPoint NumberToCoord(int index);

signals:
    void addPathPoint(QPoint newPoint);
};

#endif // PATHFINDER_H
