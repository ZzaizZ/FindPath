#include "mapfield.h"

#include <QGraphicsEllipseItem>

#include <stdio.h>

// TODO: Заменить размер ячеек (20) хотя бы на define
// TODO: добавить метод, меняющий один тип ячейки на другой
// TODO: Убрать повторяющийся код в перерисовке пути

Map::Map(int H, int W, QObject *parent) :
    QGraphicsScene (parent),
    m_w(W),
    m_h(H)
{
    generateMap(m_w, m_h);
    removeItem(map[0][0]);
    removeItem(map[m_w-1][m_h-1]);
    map[0][0] = new TextCell(20, 20, "A");
    map[m_w-1][m_h-1] = new TextCell(20, 20, "B");
    m_start = map[0][0];
    m_end = map[m_w-1][m_h-1];
    m_start->setPos(0,0);
    m_end->setPos((m_w-1)*20,(m_h-1)*20);
    addItem(m_start);
    addItem(m_end);
    FindTheWay(m_start->pos(), m_end->pos());
}

Map::~Map()
{
}

void Map::FindTheWay(QPointF p_start, QPointF p_end)
{
    adj_matrix = GetAdjMatrix(m_w, m_h, walls);
    int n_start = p_start.y()/20*m_h + p_start.x()/20;
    int n_end = p_end.y()/20*m_h + p_end.x()/20;
    path = BestPath(m_w*m_h, n_start, n_end);
    if (path[0] != PATH_DOES_NOT_EXISTS)
            for (int s = 1; s < path.size()-1; s++)
            {
                int step = path[s];
                wall_coords_t wc = NumberToCoord(step, m_w, m_h);
                removeItem(map[wc.y][wc.x]);
                map[wc.y][wc.x] = new PathCell(20, 20);
                map[wc.y][wc.x]->setPos(wc.x*20, wc.y*20);
                addItem(map[wc.y][wc.x]);
            }
        else
            printf("Path does not exist\n");
}

std::vector<wall_coords_t> Map::generateMap(int W, int H)
{
    srand(time(nullptr));
    map.resize(H);
    walls.clear();
    for (int y = 0; y < H; y++)
    {
        map[y].resize(W);
        for (int x = 0; x < W; x++)
        {
            if (rand() % 7 == 1)
            {
                map[y][x] = new WallCell(20, 20);
                map[y][x]->setPos(x*20, y*20);
                wall_coords_t wall(x, y);
                this->addItem(map[y][x]);
                walls.push_back(wall);
            }
            else
            {
                map[y][x] = new EmptyCell(20, 20);
                map[y][x]->setPos(x*20, y*20);
                wall_coords_t wall(x, y);
                this->addItem(map[y][x]);
            }
        }
    }
    return walls;
}

// TODO: ОПТИМИЗИРОВАТЬ?
std::vector<std::vector<int>> Map::GetAdjMatrix(int W, int H, std::vector<wall_coords_t> walls)
{
    size_t nodes_n = W*H;
    adj_matrix.resize(nodes_n);
    for (size_t cur_node = 0; cur_node < nodes_n; cur_node++)
    {
        adj_matrix[cur_node].resize(nodes_n);
        std::fill(adj_matrix[cur_node].begin(), adj_matrix[cur_node].end(), 0);
    }
    // генерация марицы инциденции для поля
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
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
    for (wall_coords_t wall : walls)
    {
        if (wall.x != 0)
        {
            adj_matrix[wall.y*W + wall.x][wall.y*W + wall.x-1] = 0;
            adj_matrix[wall.y*W + wall.x-1][wall.y*W + wall.x] = 0;
        }
        if (wall.x != W-1)
        {
            adj_matrix[wall.y*W + wall.x][wall.y*W + wall.x+1] = 0;
            adj_matrix[wall.y*W + wall.x+1][wall.y*W + wall.x] = 0;
        }
        if (wall.y != 0)
        {
            adj_matrix[wall.y*W + wall.x][(wall.y-1)*W + wall.x] = 0;
            adj_matrix[(wall.y-1)*W + wall.x][wall.y*W + wall.x] = 0;
        }
        if (wall.y < H-1)
        {
            adj_matrix[wall.y*W + wall.x][(wall.y+1)*W + wall.x] = 0;
            adj_matrix[(wall.y+1)*W + wall.x][wall.y*W + wall.x] = 0;
        }
    }

    return adj_matrix;
}

// возвращает координаты N ячейки сетки (нумерация с 0)
// O(n^2)
wall_coords_t Map::NumberToCoord(int index, int W, int H)
{
    if (W*H-1 < index)
        return wall_coords_t(-1, -1);
    int counter = 0;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            if (index == counter)
                return wall_coords_t(x, y);
            else
                counter++;
    return wall_coords_t(-1, -1);
}

// возвращает вектор индексов узлов, которые составляют путь от старта до финиша
// (нумерация узлов с 0)
std::vector<int> Map::BestPath(int n, int v_start, int v_end)
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
    std::vector<int> result;

    if (path_exists)
    {
        int current = v_end;
        while (current != v_start)
        {
            result.push_back(current);
            current = came_from[current];
        }
        result.push_back(v_start);
    }
    else
        result.push_back(PATH_DOES_NOT_EXISTS);
    std::reverse(result.begin(), result.end());
    return result;
}

void Map::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    // TODO: Убрать повторяющиеся куски кода
    Cell *it = nullptr;
    switch (e->button())
    {
    case Qt::LeftButton:
        if ((it = dynamic_cast<Cell*>(itemAt(e->scenePos(),QTransform()))))
        {
            if (it->getType() != CellType::Wall)
            {
                QPointF pos = it->pos();
                removeItem(it);
                int x = static_cast<int>(it->pos().x()/20);
                int y = static_cast<int>(it->pos().y()/20);
                map[y][x] = new TextCell(20, 20, "A");
                map[y][x]->setPos(pos);
                if (path[0] != PATH_DOES_NOT_EXISTS)
                    for (int s = 0; s < path.size()-1; s++)
                    {
                        int step = path[s];
                        wall_coords_t wc = NumberToCoord(step, m_w, m_h);
                        removeItem(map[wc.y][wc.x]);
                        map[wc.y][wc.x] = new EmptyCell(20, 20);
                        map[wc.y][wc.x]->setPos(wc.x*20, wc.y*20);
                        addItem(map[wc.y][wc.x]);
                    }
                m_start = map[y][x];
                FindTheWay(m_start->pos(), m_end->pos());
                addItem(map[y][x]);
            }
        }
        break;
    case Qt::RightButton:
        if ((it = dynamic_cast<Cell*>(itemAt(e->scenePos(),QTransform()))))
        {
            if (it->getType() != CellType::Wall)
            {
                QPointF pos = it->pos();
                removeItem(it);
                int x = static_cast<int>(it->pos().x()/20);
                int y = static_cast<int>(it->pos().y()/20);
                map[y][x] = new TextCell(20, 20, "B");
                map[y][x]->setPos(pos);
                if (path[0] != PATH_DOES_NOT_EXISTS)
                    for (int s = 1; s < path.size(); s++)
                    {
                        int step = path[s];
                        wall_coords_t wc = NumberToCoord(step, m_w, m_h);
                        removeItem(map[wc.y][wc.x]);
                        map[wc.y][wc.x] = new EmptyCell(20, 20);
                        map[wc.y][wc.x]->setPos(wc.x*20, wc.y*20);
                        addItem(map[wc.y][wc.x]);
                    }
                m_end = map[y][x];
                FindTheWay(m_start->pos(), m_end->pos());
                addItem(map[y][x]);
            }
        }
        break;
    default:
        break;
    }
}
