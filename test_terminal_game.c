#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10

// 方向控制
enum Direction { UP, DOWN, LEFT, RIGHT };
struct Snake {
    int x, y;
} snake[100];

int length = 5;
enum Direction dir = RIGHT;
int food_x, food_y;

// **设置终端为非阻塞模式**
void setup_terminal() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); // 关闭缓冲 & 关闭回显
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// **检查键盘输入（非阻塞）**
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// **渲染游戏界面**
void draw() {
    system("clear"); // 清屏
    for (int i = 0; i <= HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            if (i == 0 || i == HEIGHT || j == 0 || j == WIDTH) {
                printf("#"); // 边界
            } else if (i == food_y && j == food_x) {
                printf("*"); // 食物
            } else {
                int isBody = 0;
                for (int k = 0; k < length; k++) {
                    if (snake[k].x == j && snake[k].y == i) {
                        printf("O"); // 蛇身
                        isBody = 1;
                        break;
                    }
                }
                if (!isBody) printf(" ");
            }
        }
        printf("\n");
    }
}

// **更新游戏逻辑**
void update() {
    // 处理输入
    if (kbhit()) {
        char key = getchar();
        switch (key) {
            case 'w': if (dir != DOWN) dir = UP; break;
            case 's': if (dir != UP) dir = DOWN; break;
            case 'a': if (dir != RIGHT) dir = LEFT; break;
            case 'd': if (dir != LEFT) dir = RIGHT; break;
            case 'q': exit(0); // 退出
        }
    }

    // 移动蛇
    for (int i = length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    if (dir == UP) snake[0].y--;
    if (dir == DOWN) snake[0].y++;
    if (dir == LEFT) snake[0].x--;
    if (dir == RIGHT) snake[0].x++;

    // 碰撞检测
    if (snake[0].x == 0 || snake[0].x == WIDTH || snake[0].y == 0 || snake[0].y == HEIGHT) {
        printf("Game Over!\n");
        exit(0);
    }

    // 食物检测
    if (snake[0].x == food_x && snake[0].y == food_y) {
        length++;
        food_x = rand() % (WIDTH - 1) + 1;
        food_y = rand() % (HEIGHT - 1) + 1;
    }
}

// **主循环**
int main(void) {
    srand(time(NULL));
    setup_terminal();
    
    // 初始化蛇
    for (int i = 0; i < length; i++) {
        snake[i].x = 5 - i;
        snake[i].y = 5;
    }
    
    // 生成食物
    food_x = rand() % (WIDTH - 1) + 1;
    food_y = rand() % (HEIGHT - 1) + 1;

    while (1) {
        draw();
        update();
        usleep(200000); // 控制游戏速度 (200ms)
    }

    return 0;
}
