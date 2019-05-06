#include "mapfield.h"

#include <QGraphicsEllipseItem>

#include <QMessageBox>
#include <QTime>
#include <QThread>


Map::Map(int H, int W, QObject *parent) :
    QGraphicsScene (parent),
    m_w(W),
    m_h(H)
{
    qRegisterMetaType<CellType>("CellType");

    finder = new PathFinder(W, H);
    connect(this, &Map::signalGenerateMap, finder, &PathFinder::generateMap);
    connect(this, &Map::signalFindTheWay, finder, &PathFinder::findTheWay);    
    connect(finder, &PathFinder::signalAddCell, this, &Map::drawMapCell);
    connect(finder, &PathFinder::signalPathNotFound, this, &Map::errorPathNotFound);
    connect(finder, &PathFinder::signalBuisyChanged, this, &Map::changeBuisyStatus);
    thread_path_finder = new QThread();
    finder->moveToThread(thread_path_finder);
    thread_path_finder->start();

    generateMap(m_w, m_h);
    thread_is_buisy = false;
}

Map::~Map()
{
    thread_path_finder->quit();
    thread_path_finder->wait();
    delete thread_path_finder;
    delete finder;
}

void Map::generateMap(int W, int H)
{
    clearPath();
    clear();
    m_w = W; m_h = H;
    m_start = nullptr;
    m_end = nullptr;
    emit signalGenerateMap(m_w, m_h);
}

void Map::drawMapCell(QPoint mapCell, CellType cell_type)
{
    switch (cell_type)
    {
    case CellType::Wall:
        addItem(new WallCell(CELL_SIZE, CELL_SIZE, mapCell.x()*CELL_SIZE, mapCell.y()*CELL_SIZE));
        break;
    case CellType::Empty:
        addItem(new EmptyCell(CELL_SIZE, CELL_SIZE, mapCell.x()*CELL_SIZE, mapCell.y()*CELL_SIZE));
        break;
    case CellType::Path:
        path_cell.push_back(new PathCell(CELL_SIZE, CELL_SIZE, mapCell.x()*CELL_SIZE, mapCell.y()*CELL_SIZE));
        path_cell.back()->setPos(mapCell.x()*CELL_SIZE, mapCell.y()*CELL_SIZE);
        addItem(path_cell.back());
        break;
    default:
        break;
    }
}

// стирает текущий путь
void Map::clearPath()
{
    for (size_t s = 0; s < path_cell.size(); s++)
    {
        removeItem(path_cell[s]);
        delete path_cell[s];
    }
    path_cell.clear();
    update();
}

void Map::errorPathNotFound()
{
    QMessageBox::information(nullptr, "Внимание!", "Не существует пути между заданными координатами");
}

void Map::changeBuisyStatus(bool in_process)
{
    emit signalBuisyChanged(in_process);
    thread_is_buisy = in_process;
}

void Map::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (thread_is_buisy)
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
                    m_start = new TextCell(CELL_SIZE, CELL_SIZE, pos.x(), pos.y(), "A");
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
                    m_end = new TextCell(CELL_SIZE, CELL_SIZE, pos.x(), pos.y(), "B");
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
