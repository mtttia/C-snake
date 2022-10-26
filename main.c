#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>
#include <sys/types.h>
#include <unistd.h>
#include<process.h>
#include <pthread.h>
#include <windows.h>

#define ROW 10
#define COLUMN 10

#define START_X 4
#define START_Y 4

#define EMPTY_CODE 0
#define SNAKE_CODE 1
#define PEACH_CODE 2
#define EMPTY_SYMBOL 32
#define SNAKE_SYMBOL 42
#define PEACH_SYMBOL 48

#define VERTICAL_DELIMITER 124
#define HORIZONTAL_DELIMITER 95
#define CORNER 94

#define LEFT 0
#define TOP 1
#define RIGHT 2
#define DOWN 3
#define LEFT_CHAR 97
#define TOP_CHAR 119
#define RIGHT_CHAR 100
#define DOWN_CHAR 115

bool PACMAN_EFFECT = false;
//pacman effect == true is UNSTABLE

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

typedef struct{
    int x;
    int y;
} point;

int campo [ROW][COLUMN];
point snake [ROW * COLUMN];
int snakeDirection = RIGHT;
int currentPos = 1;
point peach;
bool lost = false;

void wait(int trigger)
{
    int msec = 0;
    clock_t before = clock();
    do{
        clock_t difference = clock() - before;
        msec = difference * 1000 / CLOCKS_PER_SEC;
    } while (msec < trigger);
}

int random(int min, int max)
{
    return (rand() % (max + 1 - min)) + min;
}

int genCoord()
{
    if(kbhit())
    {
        //ci sono valori nel bugger
        int ch = getch();
        switch (ch) {
            case LEFT_CHAR:
                snakeDirection = LEFT;
                break;
            case TOP_CHAR:
                snakeDirection = TOP;
                break;
            case RIGHT_CHAR:
                snakeDirection = RIGHT;
                break;
            case DOWN_CHAR:
                snakeDirection = DOWN;
                break;
        }

        fflush(stdin);
    }

    /*
    if(snake[currentPos].x > peach.x)
    {
        snakeDirection = TOP;
    }
    else if(snake[currentPos].x < peach.x)
    {
        snakeDirection = DOWN;
    }
    else if(snake[currentPos].y > peach.y)
    {
        snakeDirection = LEFT;
    }
    else if(snake[currentPos].y < peach.y)
    {
        snakeDirection = RIGHT;
    }
     */
}
void clearCamp()
{
    for(int i = 0; i < ROW; i++)
    {
        for(int j = 0; j < COLUMN; j++)
        {
            campo[i][j] = EMPTY_CODE;
        }
    }
}

void drawSnake()
{
    for(int i = 0; i <= currentPos; i++)
    {
        campo[snake[i].x][snake[i].y] = SNAKE_CODE;
    }
}

void drawPeach()
{
    campo[peach.x][peach.y] = PEACH_CODE;
}

void printCamp()
{
    gotoxy(0,0);

    clearCamp();
    drawSnake();
    drawPeach();

    printf("%c", CORNER);
    for(int i = 0; i < ROW; i++)
    {
        printf("%c", HORIZONTAL_DELIMITER);
    }
    printf("%c\n", CORNER);

    for(int i = 0; i < ROW; i++)
    {
        printf("%c", VERTICAL_DELIMITER);
        for(int j = 0; j < COLUMN; j++)
        {
            switch (campo[i][j]) {
                case SNAKE_CODE:
                    printf("%c", SNAKE_SYMBOL);
                    break;
                case PEACH_CODE:
                    printf("%c", PEACH_SYMBOL);
                    break;
                case EMPTY_CODE:
                default:
                    printf("%c", EMPTY_SYMBOL);
                    break;
            }
        }
        printf("%c", VERTICAL_DELIMITER);
        printf("\n");
    }

    printf("%c", CORNER);
    for(int i = 0; i < ROW; i++)
    {
        printf("%c", HORIZONTAL_DELIMITER);
    }
    printf("%c\n", CORNER);
}

void initMatrix()
{
    for(int i = 0; i < ROW; i++)
    {
        for(int j = 0; j < COLUMN; j++)
        {
            campo[i][j] = EMPTY_CODE;
        }
    }
}

bool peachOk(point p)
{
    for(int i = 0; i <= currentPos; i++)
    {
        if(snake[i].x == p.x && snake[i].y == p.y)
        {
            return false;
        }
    }
    return true;
}

void genPeach()
{
    point p;
    do {
        p.x = random(0, ROW-1);
        p.y = random(0, COLUMN-1);
    } while (peachOk(p) == false);
    peach = p;
}

void checkLost(point p)
{
    if(PACMAN_EFFECT == false)
    {
        if(p.x < 0 || p.x >= ROW || p.y < 0 || p.y >= COLUMN)
        {
            lost = true;
        }
    }

    if(!lost)
    {
        for(int i = 0; i <= currentPos; i++)
        {
            if(p.x == snake[i].x && p.y == snake[i].y)
            {
                lost = true;
            }
        }
    }
}

void move()
{
    bool cancel = true;

    int lastSnakeX = snake[currentPos].x;
    int lastSnakeY = snake[currentPos].y;

    ++currentPos;

    if(PACMAN_EFFECT)
    {
        switch (snakeDirection) {
            case LEFT:
                if(lastSnakeY == 0)
                    lastSnakeY = COLUMN - 1;
                else
                    --lastSnakeY;
                break;
            case RIGHT:
                if(lastSnakeY == COLUMN - 1)
                    lastSnakeY = 0;
                else
                    ++lastSnakeY;
                break;
            case TOP:
                if(lastSnakeX == 0)
                    lastSnakeX = TOP - 1;
                else
                    --lastSnakeX;
                break;
            case DOWN:
                if(lastSnakeX == TOP - 1)
                    lastSnakeX = 0;
                else
                    ++lastSnakeX;
                break;

        }
    }
    else
    {
        switch (snakeDirection) {
            case LEFT:
                --lastSnakeY;
                break;
            case RIGHT:
                ++lastSnakeY;
                break;
            case TOP:
                --lastSnakeX;
                break;
            case DOWN:
                ++lastSnakeX;
                break;
        }
    }




    point p;
    p.x = lastSnakeX;
    p.y = lastSnakeY;

    checkLost(p);

    snake[currentPos] = p;

    if(p.x == peach.x && p.y == peach.y)
    {
        cancel = false;
    }

    if(cancel == true)
    {

        for(int i = 1; i <= currentPos; i++)
        {
            snake[i-1] = snake[i];
        }
        --currentPos;
    }
    else
    {
        genPeach();
    }
}

void initSnake()
{
    point p;
    p.x = START_X;
    p.y = START_Y;
    snake[0] = p;
    point p2;
    p2.x = START_X;
    p2.y = START_Y+1;
    snake[1] = p2;
}



int main()
{
    //game init
    puts("Benvenuto su C-SNAKE\n\n");
    puts("ISTRUZIONI PER MUOVERSI: ");
    puts("a -> va a sinistra");
    puts("w -> va in alto");
    puts("d -> va a destra");
    puts("s -> va in basso\n");

    int choose, speed;

    do {
        puts("Seleziona il livello di difficolta'");
        puts("1) Facile\n2) Medio\n3) Difficile");
        scanf("%d", &choose);
    }while(choose < 1 && choose > 3);

    switch(choose)
    {
        case 3:
            speed = 200;
            break;
        case 2:
            speed = 350;
            break;
        case 1:
        default:
            speed = 570;
            break;
    }

    //inizia la partita
    system("cls");


    srand(time(NULL));

    initMatrix(campo);
    initSnake();
    genPeach();
    printCamp();

    while(lost == false)
    {
        //printf("X:%d, Y:%d\n", snake[currentPos].x, snake[currentPos].y);
        wait(speed);
        genCoord();

        move();

        printCamp();

    }

    system("cls");
    puts("MI DISPIACE TANTO, HAI PERSO HAHAHAHAHA XD XD XD");
    printf("PUNTI: %d\n", currentPos);
    puts("Press 'z' to close");
    char ch;
    while((ch = getch()) != 'z'){}

    return 0;
}