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

class Map : public QGraphicsScene
{

    Q_OBJECT

public:
    Map(int H, int W, QObject *parent);
    ~Map();
public slots:
    void generateMap(int W, int H);
private slots:
    void drawMapCell(QPoint mapCell, CellType ct);
    void errorPathNotFound();
    void changeSearchStatus(bool in_process);
private:
    PathFinder *finder {nullptr};
    std::vector<std::vector<Cell*>> map; // двумерный массив ячеек поля (для отрисовки)
    std::vector<QPoint> path; // координаты ячеек пути
    std::vector<Cell*> path_cell; // ячейки пути
    int m_w, m_h; // размеры поля в клетках
    // перевод номера узла графа в координаты ячейки
    QPoint NumberToCoord(int index, int W, int H);
    void clearPath(); // очистка вектора ячеек пути
    Cell *m_start;
    Cell *m_end;
    QThread thread_path_finder;
    bool search_in_process;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
signals:
    void signalGenerateMap(int m_width, int m_height);
    void signalFindTheWay(QPointF start, QPointF end);
    void signalBuisyChanged(bool in_process);
    void signalGenerationStatusChanged(bool in_process);
};



#endif // MAPFIELD_H
