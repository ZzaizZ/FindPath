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
    void drawMapCell(QPoint mapCell, CellType cell_type);
    void errorPathNotFound();
    void changeBuisyStatus(bool in_process);
private:
    PathFinder *finder;
    std::vector<Cell*> path_cell; // ячейки пути
    int m_w, m_h; // размеры поля в клетках
    void clearPath(); // очистка вектора ячеек пути
    Cell *m_start; // стартовая ячейка пути
    Cell *m_end; // конечная ячейка пути
    QThread *thread_path_finder; // поток для генерации карты и поиска пути
    bool thread_is_buisy; // происходит ли сейчас расчёт
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
signals:
    void signalGenerateMap(int m_width, int m_height);
    void signalFindTheWay(QPointF start, QPointF end);
    void signalBuisyChanged(bool in_process);
};



#endif // MAPFIELD_H
