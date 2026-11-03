#include <stdio.h>

#define MAX_SIZE 100

// 1. 定义坐标结构体
typedef struct {
    int x, y; // 坐标
    int step; // 从起点走到当前点的步数
} Point;

// 2. 全局变量定义
int N = 5, M = 5; // 迷宫大小
int map[5][5] = {
    {0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0}
};
int visited[5][5] = {0}; // 标记是否走过，0表示未走过，1表示走过

// 定义方向数组（精髓）：右、下、左、上
int dx[4] = {0, 1, 0, -1};
int dy[4] = {1, 0, -1, 0};

// 3. BFS 算法本体
int BFS(int startX, int startY, int endX, int endY) {
    // 初始化队列
    Point queue[MAX_SIZE * MAX_SIZE]; 
    int head = 0; // 队头指针（出队用）
    int tail = 0; // 队尾指针（入队用）

    // 起点入队
    queue[tail].x = startX;
    queue[tail].y = startY;
    queue[tail].step = 0;
    tail++;
    visited[startX][startY] = 1; // 标记起点已访问

    // 当队列不为空时，一直循环
    while (head < tail) {
        // 1. 取出队头元素（出队）
        Point current = queue[head];
        head++;

        // 2. 判断是否到达终点
        if (current.x == endX && current.y == endY) {
            return current.step; // 找到了，因为是BFS，第一次找到的一定是最短步数
        }

        // 3. 向四个方向探索（找邻居）
        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            // 判断新坐标是否合法（不越界、不是墙、没走过）
            if (nx >= 0 && nx < N && ny >= 0 && ny < M && 
                map[nx][ny] == 0 && visited[nx][ny] == 0) {
                
                // 把合法的新邻居加入队列（入队）
                queue[tail].x = nx;
                queue[tail].y = ny;
                queue[tail].step = current.step + 1; // 步数等于当前步数+1
                tail++;
                
                // 极其重要：立刻标记为已访问，防止其他点把它重复加进队列
                visited[nx][ny] = 1; 
            }
        }
    }
    
    return -1; // 队列空了还没找到，说明走不通
}

int main() {
    int ans = BFS(0, 0, 4, 4);
    printf("到达终点的最短步数为: %d\n", ans);
    return 0;
}