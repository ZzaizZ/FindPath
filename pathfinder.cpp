#include "pathfinder.h"

PathFinder::PathFinder(int w, int h, std::vector<QPoint> walls) :
    W(w),
    H(h),
    walls(walls)
{
    GetAdjMatrix();
}

QPoint PathFinder::NumberToCoord(int index)
{
    if (W*H-1 < index)
        return QPoint(-1, -1);
    return QPoint(index%W, index/W);
}

void PathFinder::GetAdjMatrix()
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
}

std::vector<QPoint> PathFinder::findTheWay(QPointF p_start, QPointF p_end)
{
    QPoint pint_start(p_start.x()/CELL_SIZE, p_start.y()/CELL_SIZE);
    QPoint pint_end(p_end.x()/CELL_SIZE, p_end.y()/CELL_SIZE);
    int n_start = pint_start.y()*(W) + pint_start.x();
    int n_end = pint_end.y()*(W) + pint_end.x();

    std::vector<int> came_from(W*H, UNDEF);
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
        for (int next = 0; next < W*H; next++)
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
        int current = n_end;
        while (current != n_start)
        {
            result.push_back(NumberToCoord(current));
            current = came_from[current];
        }
        result.push_back(NumberToCoord(n_start));
    }
    else
    {
        result.push_back(NumberToCoord(n_start));
        result.push_back(NumberToCoord(n_end));
    }
    std::reverse(result.begin(), result.end());
    return result;
}
