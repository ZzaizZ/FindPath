#include "pathfinder.h"

PathFinder::PathFinder(int w, int h) :
    map_width(w),
    map_height(h)
{
}

void PathFinder::generateMap(int m_width, int m_height)
{
    emit signalBuisyChanged(false);
    map_width = m_width;
    map_height = m_height;
    qsrand(QTime::currentTime().msec());
    walls.clear();
    for (size_t y = 0; y < map_height; y++)
    {
        for (size_t x = 0; x < map_width; x++)
        {
            if (rand() % 3 == 1)
            {
                emit signalAddCell(QPoint(x, y), CellType::Wall);
                walls.push_back(QPoint(x,y));
            }
            else
                emit signalAddCell(QPoint(x, y), CellType::Empty);
        }
    }
    getAdjMatrix();
    emit signalBuisyChanged(false);
}

QPoint PathFinder::numberToCoord(int index)
{
    if (map_width*map_height-1 < index)
        return QPoint(-1, -1);
    return QPoint(index%map_width, index/map_width);
}

void PathFinder::getAdjMatrix()
{
    size_t nodes_n = map_width*map_height;
    adj_matrix.resize(nodes_n);
    for (size_t cur_node = 0; cur_node < nodes_n; cur_node++)
    {
        adj_matrix[cur_node].resize(nodes_n);
        std::fill(adj_matrix[cur_node].begin(), adj_matrix[cur_node].end(), 0);
    }
    // генерация марицы инциденции для поля без препятствий
    for (size_t y = 0; y < map_height; y++)
        for (size_t x = 0; x < map_width; x++)
        {
            if (x != 0)
                adj_matrix[y*map_width + x][y*map_width + x-1] = 1;
            if (x != map_width-1)
                adj_matrix[y*map_width + x][y*map_width + x+1] = 1;
            if (y != 0)
                adj_matrix[y*map_width + x][(y-1)*map_width + x] = 1;
            if (y < map_height-1)
                adj_matrix[y*map_width + x][(y+1)*map_width + x] = 1;
        }
    // затираем связи между обычными узлами и узлами-стенами
    for (QPoint wall : walls)
    {
        if (wall.x() != 0)
        {
            adj_matrix[wall.y()*map_width + wall.x()][wall.y()*map_width + wall.x()-1] = 0;
            adj_matrix[wall.y()*map_width + wall.x()-1][wall.y()*map_width + wall.x()] = 0;
        }
        if (wall.x() != map_width-1)
        {
            adj_matrix[wall.y()*map_width + wall.x()][wall.y()*map_width + wall.x()+1] = 0;
            adj_matrix[wall.y()*map_width + wall.x()+1][wall.y()*map_width + wall.x()] = 0;
        }
        if (wall.y() != 0)
        {
            adj_matrix[wall.y()*map_width + wall.x()][(wall.y()-1)*map_width + wall.x()] = 0;
            adj_matrix[(wall.y()-1)*map_width + wall.x()][wall.y()*map_width + wall.x()] = 0;
        }
        if (wall.y() < map_height-1)
        {
            adj_matrix[wall.y()*map_width + wall.x()][(wall.y()+1)*map_width + wall.x()] = 0;
            adj_matrix[(wall.y()+1)*map_width + wall.x()][wall.y()*map_width + wall.x()] = 0;
        }
    }
}

void PathFinder::findTheWay(QPointF p_start, QPointF p_end)
{
    emit signalBuisyChanged(true);
    QPoint pint_start(p_start.x()/CELL_SIZE, p_start.y()/CELL_SIZE);
    QPoint pint_end(p_end.x()/CELL_SIZE, p_end.y()/CELL_SIZE);
    int n_start = pint_start.y()*(map_width) + pint_start.x();
    int n_end = pint_end.y()*(map_width) + pint_end.x();
    std::vector<int> came_from(map_width*map_height, UNDEF);
    std::queue<int> q;
    q.push(n_start);
    came_from[n_start] = 0;
    bool path_exists = false;
    while(!q.empty())
    {
        int cur = q.front();
        q.pop();
        if (cur == n_end)
        {
            path_exists = true;
            break;
        }
        for (int next = 0; next < map_width*map_height; next++)
        {
            if (came_from[next] == UNDEF && adj_matrix[cur][next] == 1)
            {
                q.push(next);
                came_from[next] = cur;
            }
        }
    }

    if (path_exists)
    {
        if (n_start == n_end)
            return;
        int current = n_end;
        current = came_from[current];
        while (current != n_start)
        {
            emit signalAddCell(numberToCoord(current), CellType::Path);
            current = came_from[current];
        }
    }
    else
        emit signalPathNotFound();
    emit signalBuisyChanged(false);
}
