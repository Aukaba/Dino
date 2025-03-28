#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

// Global variables
int y = 15, jump = 0, score = 0, speed = 90, color = 1;
int high_score = 0;
int is_jumping = 0;
int obstacle_x = 80;
int obstacle_width = 5;
int game_active = 1;
clock_t last_score_time;

// Function declarations
void textcolor(int color);
void gotoxy(int x, int y);
void delay(int milliseconds);
void load_high_score();
void save_high_score();
void dinosour();
void Score();
void draw_environment();
int check_collision();
void move_obstacle();
void jump_animation();
void game_over();
void game_loop();

// Custom functions implementation
void textcolor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void delay(int milliseconds) {
    Sleep(milliseconds);
}

// High Score functions
void load_high_score() {
    FILE* file = fopen("highscore.txt", "r");
    if (file == NULL) return;
    fscanf(file, "%d", &high_score);
    fclose(file);
}

void save_high_score() {
    FILE* file = fopen("highscore.txt", "w");
    if (file == NULL) return;
    fprintf(file, "%d", high_score);
    fclose(file);
}

// Dinosaur drawing function
void dinosour() {
    textcolor(15);
    gotoxy(2, y);
    printf("           UUssUUU     ");
    gotoxy(2, y+1);
    printf("           UUUUssss        ");
    gotoxy(2, y+2);
    printf("           UUUUUs       ");
    gotoxy(2, y+3);
    printf(" U        UUUUU");
    gotoxy(2, y+4);
    printf(" UUU    UUUUUUss");
    gotoxy(2, y+5);
    printf("  UUUUUUUUUUU  s");
    gotoxy(2, y+6);
    printf("  sUUUUUUUs");
    
    if(jump == 2) {
        gotoxy(2, y+7);
        printf("    UUs");
        gotoxy(2, y+8);
        printf("    UU  U");
    }
    else if(jump == 0) {
        gotoxy(2, y+7);
        printf("    Uss");
        gotoxy(2, y+8);
        printf("    UU ");
        jump = 1;
    }
    else if(jump == 1) {
        gotoxy(2, y+7);
        printf("   UUs");
        gotoxy(2, y+8);
        printf("    s  UU ");
        jump = 0;
    }
}

void Score() {
    textcolor(3);
    gotoxy(60, 5);
    printf("Score = %d", score);
    gotoxy(60, 8);
    printf("QUIT = ESC KEY");
    gotoxy(60, 2);
    printf("Jump = Space");
    gotoxy(60, 11);
    printf("High Score: %d", high_score);
}

void draw_environment() {
    int i;
    textcolor(2);
    for(i = 5; i <= 80; i++) {
        gotoxy(i, 24);
        printf("_");
    }
    
    textcolor(10);
    if(obstacle_x <= 80 && obstacle_x >= 0) {
        gotoxy(obstacle_x, 22);
        printf("|\\|");
        gotoxy(obstacle_x, 23);
        printf("| |");
    }
}

int check_collision() {
    if(obstacle_x <= 7 && obstacle_x >= 2 && y >= 13) {
        return 1;
    }
    return 0;
}

void move_obstacle() {
    obstacle_x--;
    if(obstacle_x < -obstacle_width) {
        obstacle_x = 80;
        color++;
        if(color > 15) color = 1;
    }
}

void game_over() {
    if(score > high_score) {
        high_score = score;
        save_high_score();
    }
    gotoxy(30, 12);
    printf("Game over! Press any key to continue");
    game_active = 0;
}

void jump_animation() {
    is_jumping = 1;
    
    // Jump up
    for(int j = 0; j < 4 && game_active; j++) {
        system("cls");
        y -= 2;
        move_obstacle();
        draw_environment();
        dinosour();
        Score();
        if(check_collision()) game_over();
        delay(speed - 20);
    }
    
    // Hang time
    for(int j = 0; j < 10 && game_active; j++) {
        system("cls");
        move_obstacle();
        draw_environment();
        dinosour();
        Score();
        if(check_collision()) game_over();
        delay(speed);
    }
    
    // Fall down
    for(int j = 0; j < 4 && game_active; j++) {
        system("cls");
        y += 2;
        move_obstacle();
        draw_environment();
        dinosour();
        Score();
        if(check_collision()) game_over();
        delay(speed - 20);
    }
    
    is_jumping = 0;
    jump = 0;
}

void game_loop() {
    load_high_score();
    last_score_time = clock();
    
    while(game_active) {
        clock_t current_time = clock();
        
        // Increment score based on time passed
        if((current_time - last_score_time) > (CLOCKS_PER_SEC / 10)) {
            score++;
            last_score_time = current_time;
        }

        system("cls");
        Score();
        draw_environment();
        dinosour();
        
        move_obstacle();
        
        if(check_collision()) {
            game_over();
            break;
        }
        
        // Increase speed every 100 points
        if(score % 100 == 0 && score > 0) {
            speed -= 5;
            if(speed < 25) speed = 25;
        }
        
        if(kbhit()) {
            int A = getch();
            if(A == ' ' && !is_jumping) {
                jump_animation();
            }
            else if(A == 27) {
                exit(0);
            }
        }
        
        delay(speed);
    }
    
    // Reset game state after game over
    getch();
    y = 15;
    score = 0;
    speed = 90;
    obstacle_x = 80;
    game_active = 1;
}

int main() {
    while(1) {
        system("cls");
        printf("===== DINOSAUR GAME =====\n");
        printf("Press any key to start\n");
        printf("ESC to quit\n");
        
        int choice = getch();
        if(choice == 27) exit(0);
        
        game_loop();
    }
    return 0;
}
