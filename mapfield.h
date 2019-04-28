#ifndef MAPFIELD_H
#define MAPFIELD_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <vector>
#include <queue>

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

#include "cell.h"
#include "pathfinder.h"

#define UNDEF -1 // обозначение ещё не посещённой вершины
#define CELL_SIZE 20

class Map : public QGraphicsScene
{

    Q_OBJECT

public:
    Map(int H, int W, QObject *parent);
    ~Map();
    void generateMap(int W, int H);
public slots:
    void findTheWay(QPointF p_start, QPointF p_end);
    void drawPathCell(QPoint pathCell);
    void errorPathNotFound();
private:
    PathFinder *finder {nullptr};
    std::vector<std::vector<Cell*>> map; // двумерный массив ячеек поля (для отрисовки)
    std::vector<QPoint> walls; // массив координат со стенами
    std::vector<QPoint> path; // координаты ячеек пути
    std::vector<Cell*> path_cell; // ячейки пути
    int m_w, m_h; // размеры поля в клетках
    // перевод номера узла графа в координаты ячейки
    QPoint NumberToCoord(int index, int W, int H);
    void clearPath(); // очистка вектора ячеек пути
    Cell *m_start;
    Cell *m_end;
    QThread thread_path_finder;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
signals:
    void signalFindTheWay(QPointF start, QPointF end);
};



#endif // MAPFIELD_H
