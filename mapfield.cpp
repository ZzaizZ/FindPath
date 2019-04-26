#include "mapfield.h"

#include <QGraphicsEllipseItem>

#include <stdio.h>
#include <QMessageBox>
#include <QTime>


Map::Map(int H, int W, QObject *parent) :
    QGraphicsScene (parent),
    m_w(W),
    m_h(H)
{
    generateMap(m_w, m_h);
}

Map::~Map()
{
    if (finder)
        delete finder;
}

void Map::generateMap(int W, int H)
{
    m_start = nullptr;
    m_end = nullptr;
    qsrand(QTime::currentTime().msec());
    map.resize(H);
    walls.clear();
    for (size_t y = 0; y < H; y++)
    {
        map[y].resize(W);
        for (size_t x = 0; x < W; x++)
        {
            if (rand() % 3 == 1)
            {
                map[y][x] = new WallCell(CELL_SIZE, CELL_SIZE);
                map[y][x]->setPos(x*CELL_SIZE, y*CELL_SIZE);
                this->addItem(map[y][x]);
                walls.push_back(QPoint(x,y));
            }
            else
            {
                map[y][x] = new EmptyCell(CELL_SIZE, CELL_SIZE);
                map[y][x]->setPos(x*CELL_SIZE, y*CELL_SIZE);
                this->addItem(map[y][x]);
            }
        }
    }
    finder = new PathFinder(W, H, walls);
    m_w = W; m_h = H;    
}

void Map::FindTheWay(QPointF p_start, QPointF p_end)
{
    if (!m_start || !m_end)
        return;
    path = finder->findTheWay(p_start, p_end);
    path_cell.resize(path.size());
    for (size_t s = 1; s < path.size()-1; s++)
    {
        QPoint wc = path[s];        
        if (s > 0 && s < path.size()-1)
        {
            path_cell[s] = new PathCell(CELL_SIZE, CELL_SIZE);
            path_cell[s]->setPos(wc.x()*CELL_SIZE, wc.y()*CELL_SIZE);
        }
        addItem(path_cell[s]);
    }
}

// стирает текущий путь
// кроме точек старта и финиша
void Map::ClearPath()
{
    for (int s = 1; s < path_cell.size()-1; s++)
        removeItem(path_cell[s]);
}

void Map::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    Cell *it = nullptr;
    switch (e->button())
    {
    case Qt::LeftButton:
        if ((it = dynamic_cast<Cell*>(itemAt(e->scenePos(),QTransform()))))
        {
            if (it->getType() != CellType::Wall)
            {
                if (m_start == nullptr)
                {
                    QPointF pos = it->pos();
                    m_start = new TextCell(CELL_SIZE, CELL_SIZE, "A");
                    m_start->setPos(pos);
                    addItem(m_start);
                }
                else
                {
                    if (path.size() != 0)
                        ClearPath();
                    QPointF pos = it->pos();
                    m_start->setPos(pos);
                }
                if (m_end != nullptr)
                    FindTheWay(QPointF(m_start->pos().x(), m_start->pos().y()),
                               QPointF(m_end->pos().x(), m_end->pos().y()));
            }
        }
        break;
    case Qt::RightButton:
        if ( (it = dynamic_cast<Cell*>(itemAt(e->scenePos(),QTransform()))) )
        {
            if (it->getType() != CellType::Wall)
            {
                if (m_end == nullptr)
                {
                    QPointF pos = it->pos();
                    m_end = new TextCell(CELL_SIZE, CELL_SIZE, "B");
                    m_end->setPos(pos);
                    addItem(m_end);
                }
                else
                {
                    if (path.size() != 0)
                        ClearPath();
                    QPointF pos = it->pos();
                    m_end->setPos(pos);
                }
                if (m_start != nullptr)
                    FindTheWay(QPointF(m_start->pos().x(), m_start->pos().y()),
                               QPointF(m_end->pos().x(), m_end->pos().y()));
            }
        }
        break;
    default:
        break;
    }
}
