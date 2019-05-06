#include "pathfinder.h"

PathFinder::PathFinder(int w, int h) :
    map_width(w),
    map_height(h)
{
}

void PathFinder::generateMap(int m_width, int m_height)
{
    emit signalBuisyChanged(true);
    map_width = m_width;
    map_height = m_height;
    qsrand(QTime::currentTime().msec());

    size_t nodes_n = map_width*map_height;
    adj_matrix.resize(nodes_n);
    for (size_t cur_node = 0; cur_node < nodes_n; cur_node++)
    {
        adj_matrix[cur_node].resize(nodes_n);
        std::fill(adj_matrix[cur_node].begin(), adj_matrix[cur_node].end(), 0);
    }

    bool** tmp_wall = new bool*[map_height];
    for (size_t y = 0; y < map_height; y++)
    {
        tmp_wall[y] = new bool[map_width];
        for (size_t x = 0; x < map_width; x++)
        {
            tmp_wall[y][x] = false;
            QPoint current(x, y);
            size_t number_left = coordToNumber(QPoint(x-1, y)),
                    number_top = coordToNumber(QPoint (x, y-1)),
                    number_current = coordToNumber(current);
            if (rand() % 3 == 1)
            {
                emit signalAddCell(current, CellType::Wall);
                tmp_wall[y][x] = true;
            }
            else
            {
                emit signalAddCell(current, CellType::Empty);
                tmp_wall[y][x] = false;
                // установка связей между вершинами графа
                if (x != 0 && !tmp_wall[y][x-1])
                {
                    adj_matrix[number_current][number_left] = 1;
                    adj_matrix[number_left][number_current] = 1;
                }
                if (y != 0 && !tmp_wall[y-1][x])
                {
                    adj_matrix[number_current][number_top] = 1;
                    adj_matrix[number_top][number_current] = 1;
                }
            }

        }
    }
    for (size_t y = 0; y < map_height; y++)
        delete[] tmp_wall[y];
    delete[] tmp_wall;
    emit signalBuisyChanged(false);
}

QPoint PathFinder::numberToCoord(int index)
{
    if (map_width*map_height-1 < index)
        return QPoint(-1, -1);
    return QPoint(index%map_width, index/map_width);
}

size_t PathFinder::coordToNumber(QPoint point)
{
    return point.y() * map_width + point.x();
}

void PathFinder::findTheWay(QPointF p_start, QPointF p_end)
{
    emit signalBuisyChanged(true);
    QPoint pint_start(p_start.x()/CELL_SIZE, p_start.y()/CELL_SIZE);
    QPoint pint_end(p_end.x()/CELL_SIZE, p_end.y()/CELL_SIZE);
    int n_start = pint_start.y()*(map_width) + pint_start.x();
    int n_end = pint_end.y()*(map_width) + pint_end.x();
    if (n_start == n_end)
    {
        emit signalBuisyChanged(false);
        return;
    }
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
