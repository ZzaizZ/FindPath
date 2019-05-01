#include "mapfield.h"

#include <QGraphicsEllipseItem>

#include <stdio.h>
#include <QMessageBox>
#include <QTime>
#include <QThread>

#include <QDebug>


Map::Map(int H, int W, QObject *parent) :
    QGraphicsScene (parent),
    m_w(W),
    m_h(H)
{
    generateMap(m_w, m_h);
    search_in_process = false;
}

Map::~Map()
{
    thread_path_finder.quit();
    if (finder)
        delete finder;
}

void Map::generateMap(int W, int H)
{
    emit signalGenerationStatusChanged(true);
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
    if (finder)
    {
        disconnect(this, &Map::signalFindTheWay, finder, &PathFinder::findTheWay);
        disconnect(finder, &PathFinder::signalAddPathPoint, this, &Map::drawPathCell);
        disconnect(finder, &PathFinder::signalPathNotFound, this, &Map::errorPathNotFound);
        disconnect(finder, &PathFinder::signalSearchstatusChanged, this, &Map::changeSearchStatus);
        thread_path_finder.quit();
        delete finder;
        finder = nullptr;
    }
    finder = new PathFinder(W, H, walls);
    connect(this, &Map::signalFindTheWay, finder, &PathFinder::findTheWay);
    connect(finder, &PathFinder::signalAddPathPoint, this, &Map::drawPathCell);
    connect(finder, &PathFinder::signalPathNotFound, this, &Map::errorPathNotFound);
    connect(finder, &PathFinder::signalSearchstatusChanged, this, &Map::changeSearchStatus);
    finder->moveToThread(&thread_path_finder);
    thread_path_finder.start();
    m_w = W; m_h = H;
    emit signalGenerationStatusChanged(false);
}

void Map::drawPathCell(QPoint pathCell)
{
    path_cell.push_back(new PathCell(CELL_SIZE, CELL_SIZE));
    path_cell.back()->setPos(pathCell.x()*CELL_SIZE, pathCell.y()*CELL_SIZE);
    addItem(path_cell.back());
}

void Map::findTheWay(QPointF p_start, QPointF p_end)
{
    if (!m_start || !m_end)
        return;
    finder->findTheWay(p_start, p_end);
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
void Map::clearPath()
{
    for (int s = 0; s < path_cell.size(); s++)
        removeItem(path_cell[s]);
    path_cell.clear();
    update();
}

void Map::errorPathNotFound()
{
    QMessageBox::information(nullptr, "Внимание!", "Не существует пути между заданными координатами");
}

void Map::changeSearchStatus(bool in_process)
{
    emit signalSearchstatusChanged(in_process);
    search_in_process = in_process;
}

void Map::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (search_in_process)
        return;
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
                    if (!path_cell.empty())
                        clearPath();
                    QPointF pos = it->pos();
                    m_start->setPos(pos);
                }
                if (m_end != nullptr)
                    emit signalFindTheWay(QPointF(m_start->pos().x(), m_start->pos().y()),
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
                    if (!path_cell.empty())
                        clearPath();
                    QPointF pos = it->pos();
                    m_end->setPos(pos);
                }
                if (m_start != nullptr)
                    emit signalFindTheWay(QPointF(m_start->pos().x(), m_start->pos().y()),
                                          QPointF(m_end->pos().x(), m_end->pos().y()));
            }
        }
        break;
    default:
        break;
    }
}
