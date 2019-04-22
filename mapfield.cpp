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
    m_start = nullptr;
    m_end = nullptr;
}

Map::~Map()
{
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
    adj_matrix = GetAdjMatrix(W, H, walls);
    m_w = W; m_h = H;
}

std::vector<std::vector<int>> Map::GetAdjMatrix(int W, int H, std::vector<QPoint> walls)
{
    size_t nodes_n = W*H;
    adj_matrix.resize(nodes_n);
    for (size_t cur_node = 0; cur_node < nodes_n; cur_node++)
    {
        adj_matrix[cur_node].resize(nodes_n);
        std::fill(adj_matrix[cur_node].begin(), adj_matrix[cur_node].end(), 0);
    }
    // генерация марицы инциденции для поля без препятствий
    for (size_t y = 0; y < H; y++)
        for (size_t x = 0; x < W; x++)
        {
            if (x != 0)
                adj_matrix[y*W + x][y*W + x-1] = 1;
            if (x != W-1)
                adj_matrix[y*W + x][y*W + x+1] = 1;
            if (y != 0)
                adj_matrix[y*W + x][(y-1)*W + x] = 1;
            if (y < H-1)
                adj_matrix[y*W + x][(y+1)*W + x] = 1;
        }
    // затираем связи между обычными узлами и узлами-стенами
    for (QPoint wall : walls)
    {
        if (wall.x() != 0)
        {
            adj_matrix[wall.y()*W + wall.x()][wall.y()*W + wall.x()-1] = 0;
            adj_matrix[wall.y()*W + wall.x()-1][wall.y()*W + wall.x()] = 0;
        }
        if (wall.x() != W-1)
        {
            adj_matrix[wall.y()*W + wall.x()][wall.y()*W + wall.x()+1] = 0;
            adj_matrix[wall.y()*W + wall.x()+1][wall.y()*W + wall.x()] = 0;
        }
        if (wall.y() != 0)
        {
            adj_matrix[wall.y()*W + wall.x()][(wall.y()-1)*W + wall.x()] = 0;
            adj_matrix[(wall.y()-1)*W + wall.x()][wall.y()*W + wall.x()] = 0;
        }
        if (wall.y() < H-1)
        {
            adj_matrix[wall.y()*W + wall.x()][(wall.y()+1)*W + wall.x()] = 0;
            adj_matrix[(wall.y()+1)*W + wall.x()][wall.y()*W + wall.x()] = 0;
        }
    }
    return adj_matrix;
}

// возвращает координаты index ячейки сетки (нумерация с 0)
// O(n^2)
QPoint Map::NumberToCoord(int index, int W, int H)
{
    if (W*H-1 < index)
        return QPoint(-1, -1);
    int counter = 0;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            if (index == counter)
                return QPoint(x, y);
            else
                counter++;
    return QPoint(-1, -1);
}

void Map::FindTheWay(QPointF p_start, QPointF p_end)
{
    if (!m_start || !m_end)
        return;
    QPoint pint_start(p_start.x()/CELL_SIZE, p_start.y()/CELL_SIZE);
    QPoint pint_end(p_end.x()/CELL_SIZE, p_end.y()/CELL_SIZE);
    int n_start = pint_start.y()*(m_w) + pint_start.x();
    int n_end = pint_end.y()*(m_w) + pint_end.x();
    path = BestPath(m_w*m_h, n_start, n_end);
    path_cell.resize(path.size());
    for (size_t s = 1; s < path.size()-1; s++)
    {
        QPoint wc = path[s];
        if (s == 0)
            path_cell[s] = m_start;
        if (s == path.size()-1)
            path_cell[s] = m_end;
        if (s > 0 && s < path.size()-1)
        {
            path_cell[s] = new PathCell(CELL_SIZE, CELL_SIZE);
            path_cell[s]->setPos(wc.x()*CELL_SIZE, wc.y()*CELL_SIZE);
        }
        addItem(path_cell[s]);
    }
}

// возвращает вектор координат узлов, которые составляют путь от старта до финиша
std::vector<QPoint> Map::BestPath(int n, int v_start, int v_end)
{
    std::vector<int> came_from(n, UNDEF);
    std::queue<int> q;
    q.push(v_start);
    came_from[v_start] = 0;
    bool path_exists = false;
    while(!q.empty())
    {
        int cur = q.front();
        q.pop();
        if (cur == v_end)
        {
            path_exists = true;
            break;
        }
        for (int next = 0; next < n; next++)
        {
            if (came_from[next] == UNDEF && adj_matrix[cur][next] == 1)
            {
                q.push(next);
                came_from[next] = cur;
            }
        }
    }
    std::vector<QPoint> result(0);

    if (path_exists)
    {
        int current = v_end;
        while (current != v_start)
        {
            result.push_back(NumberToCoord(current, m_w, m_h));
            current = came_from[current];
        }
        result.push_back(NumberToCoord(v_start, m_w, m_h));
    }
    else
    {
        result.push_back(NumberToCoord(v_start, m_w, m_h));
        result.push_back(NumberToCoord(v_end, m_w, m_h));
    }
    std::reverse(result.begin(), result.end());
    return result;
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
        if ((it = dynamic_cast<Cell*>(itemAt(e->scenePos(),QTransform()))))
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
