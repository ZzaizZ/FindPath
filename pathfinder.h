#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QObject>
#include <vector>
#include <queue>
#include <QPoint>

#define UNDEF -1 // обозначение ещё не посещённой вершины
#define CELL_SIZE 20

class PathFinder : public QObject
{

    Q_OBJECT

public:
    PathFinder(int w, int h, std::vector<QPoint> walls);
public slots:
    void findTheWay(QPointF p_start, QPointF p_end);
private:
    int map_width, map_height;
    std::vector<QPoint> walls;
    std::vector<std::vector<int>> adj_matrix; // матрица инциденции для поля
    void getAdjMatrix();
    QPoint numberToCoord(int index);

signals:
    void signalAddPathPoint(QPoint newPoint);
};

#endif // PATHFINDER_H
