#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>
int dino_color = 15;
typedef struct {
    char username[50];
    char password[50];
    int high_score;
} Account;

typedef struct LeaderboardNode {
    char username[50];
    int score;
    struct LeaderboardNode* next;
} LeaderboardNode;

int y = 15, jump = 0, score = 0, speed = 90, color = 1;
int is_jumping = 0;
int obstacle_x = 80;
int obstacle_width = 5;
int game_active = 1;
clock_t last_score_time;
Account current_user;
LeaderboardNode* leaderboard = NULL;

void textcolor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void gotoxy(int x, int y) {
    COORD coord = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void delay(int milliseconds) {
    Sleep(milliseconds);
}

void clear_screen() {
	int i;
    for ( i = 0; i < 30; i++) {
        gotoxy(0, i);
        printf("                                                                                "); // 80+ spaces
    }
    gotoxy(0, 0);
}

void save_account(Account account) {
    FILE* file = fopen("accounts.txt", "a");
    if (!file) return;
    fprintf(file, "%s %s %d\n", account.username, account.password, account.high_score);
    fclose(file);
}

Account load_account(char* username) {
    FILE* file = fopen("accounts.txt", "r");
    Account account = { "", "", 0 };
    if (file) {
        while (fscanf(file, "%s %s %d", account.username, account.password, &account.high_score) != EOF) {
            if (strcmp(account.username, username) == 0) {
                fclose(file);
                return account;
            }
        }
        fclose(file);
    }
    return account;
}

int username_exists(char* username) {
    FILE* file = fopen("accounts.txt", "r");
    if (!file) return 0;
    char stored_username[50];
    while (fscanf(file, "%s", stored_username) != EOF) {
        fscanf(file, "%*s %*d");
        if (strcmp(stored_username, username) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void register_account() {
    Account new_account;
    clear_screen();
    printf("===== REGISTRATION =====\n");
    do {
        printf("Enter username (no spaces): ");
        scanf("%s", new_account.username);
        if (username_exists(new_account.username)) {
            printf("Username already exists. Please choose another.\n");
        }
    } while (username_exists(new_account.username));
    printf("Enter password: ");
    scanf("%s", new_account.password);
    new_account.high_score = 0;
    save_account(new_account);
    printf("Registration successful! Press any key to continue...\n");
    getch();
}

int login() {
    char username[50], password[50];
    clear_screen();
    printf("===== LOGIN =====\n");
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    Account account = load_account(username);
    if (strcmp(account.username, "") == 0) {
        printf("Account not found. Press any key to continue...\n");
        getch();
        return 0;
    }
    if (strcmp(account.password, password) == 0) {
        current_user = account;
        printf("Login successful! Welcome %s. Press any key to continue...\n", username);
        getch();
        return 1;
    } else {
        printf("Incorrect password. Press any key to continue...\n");
        getch();
        return 0;
    }
}

void add_to_leaderboard(char* username, int score) {
    LeaderboardNode* current = leaderboard;
    LeaderboardNode* prev = NULL;

    while (current) {
        if (strcmp(current->username, username) == 0) {
            
            if (score > current->score) {
                current->score = score;
            }
            return; 
        }
        prev = current;
        current = current->next;
    }

    LeaderboardNode* new_node = (LeaderboardNode*)malloc(sizeof(LeaderboardNode));
    strcpy(new_node->username, username);
    new_node->score = score;
    new_node->next = leaderboard;
    leaderboard = new_node;

    LeaderboardNode* sorted = NULL;
    current = leaderboard;
    while (current) {
        LeaderboardNode* next = current->next;

        if (!sorted || current->score > sorted->score) {
            current->next = sorted;
            sorted = current;
        } else {
            LeaderboardNode* s = sorted;
            while (s->next && s->next->score >= current->score) {
                s = s->next;
            }
            current->next = s->next;
            s->next = current;
        }

        current = next;
    }
    leaderboard = sorted;

    current = leaderboard;
    int count = 1;
    while (current && current->next && count < 10) {
        current = current->next;
        count++;
    }
    if (current) {
        LeaderboardNode* to_free = current->next;
        current->next = NULL;
        while (to_free) {
            LeaderboardNode* temp = to_free;
            to_free = to_free->next;
            free(temp);
        }
    }
}

void load_leaderboard() {
    FILE* file = fopen("leaderboard.txt", "r");
    if (!file) return;
    char username[50];
    int score;
    while (fscanf(file, "%s %d", username, &score) != EOF) {
        add_to_leaderboard(username, score);
    }
    fclose(file);
}

void save_leaderboard() {
    FILE* file = fopen("leaderboard.txt", "w");
    if (!file) return;
    LeaderboardNode* current = leaderboard;
    while (current) {
        fprintf(file, "%s %d\n", current->username, current->score);
        current = current->next;
    }
    fclose(file);
}

void display_leaderboard() {
    clear_screen();
    printf("===== LEADERBOARD =====\n");
    printf("Rank  Username         Score\n");
    printf("--------------------------\n");
    LeaderboardNode* current = leaderboard;
    int rank = 1;
    while (current && rank <= 10) {
        printf("%-5d %-15s %d\n", rank++, current->username, current->score);
        current = current->next;
    }
    printf("\nPress any key to continue...");
    getch();
}

void free_leaderboard() {
    LeaderboardNode* current = leaderboard;
    while (current) {
        LeaderboardNode* next = current->next;
        free(current);
        current = next;
    }
    leaderboard = NULL;
}

void load_high_score() {
    current_user.high_score = load_account(current_user.username).high_score;
}

void save_high_score() {
    FILE* file = fopen("accounts.txt", "r");
    FILE* temp_file = fopen("temp.txt", "w");
    if (!file || !temp_file) return;
    Account account;
    while (fscanf(file, "%s %s %d", account.username, account.password, &account.high_score) != EOF) {
        if (strcmp(account.username, current_user.username) == 0) {
            account.high_score = current_user.high_score;
        }
        fprintf(temp_file, "%s %s %d\n", account.username, account.password, account.high_score);
    }
    fclose(file);
    fclose(temp_file);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");
}

void dinosour() {
    textcolor(dino_color);
    gotoxy(2, y);     printf("           UUssUUU     ");
    gotoxy(2, y+1);   printf("           UUUUssss        ");
    gotoxy(2, y+2);   printf("           UUUUUs       ");
    gotoxy(2, y+3);   printf(" U        UUUUU");
    gotoxy(2, y+4);   printf(" UUU    UUUUUUss");
    gotoxy(2, y+5);   printf("  UUUUUUUUUUU  s");
    gotoxy(2, y+6);   printf("  sUUUUUUUs");
    if (jump == 2) {
        gotoxy(2, y+7); printf("    UUs");
        gotoxy(2, y+8); printf("    UU  U");
    } else if (jump == 0) {
        gotoxy(2, y+7); printf("    Uss");
        gotoxy(2, y+8); printf("    UU ");
        jump = 1;
    } else if (jump == 1) {
        gotoxy(2, y+7); printf("   UUs");
        gotoxy(2, y+8); printf("    s  UU ");
        jump = 0;
    }
}

void Score() {
    textcolor(3);
    gotoxy(60, 2);  printf("Player: %s", current_user.username);
    gotoxy(60, 5);  printf("Score = %d", score);
    gotoxy(60, 8);  printf("QUIT = ESC KEY");
    gotoxy(60, 11); printf("High Score: %d", current_user.high_score);
    gotoxy(60, 14); printf("Jump = Space");
}

void draw_environment() {
    textcolor(6); // changed from 2 (green) to 6 (yellow/brown)
    int i;
    for (i = 5; i <= 80; i++) {
        gotoxy(i, 24);
        printf("_");
    }
    textcolor(10); // cactus remains green
    if (obstacle_x <= 80 && obstacle_x >= 0) {
        gotoxy(obstacle_x, 22); printf("|\\|");
        gotoxy(obstacle_x, 23); printf("| |");
    }
}


int check_collision() {
    return (obstacle_x <= 7 && obstacle_x >= 2 && y >= 13);
}

void move_obstacle() {
    obstacle_x--;
    if (obstacle_x < -obstacle_width) {
        obstacle_x = 80;
        if (++color > 15) color = 1;
    }
}

void game_over() {
    if (score > current_user.high_score) {
        current_user.high_score = score;
        save_high_score();
        add_to_leaderboard(current_user.username, current_user.high_score);
        save_leaderboard();
    }
    gotoxy(30, 12);
    printf("Game over! Press any key to continue");
    game_active = 0;
}

void jump_animation() {
    is_jumping = 1;
    int j, i;
    for (j = 0; j < 4 && game_active; j++) {
        clear_screen();
        y -= 2;
        move_obstacle();
        draw_environment();
        dinosour();
        Score();
        if (check_collision()) game_over();
        delay(speed - 20);
    }
    for (j = 0; j < 10 && game_active; j++) {
        clear_screen();
        move_obstacle();
        draw_environment();
        dinosour();
        Score();
        if (check_collision()) game_over();
        delay(speed);
    }
    
    for (j = 0; j < 4 && game_active; j++) {
        clear_screen();
        y += 2;
        move_obstacle();
        draw_environment();
        dinosour();
        Score();
        if (check_collision()) game_over();
        delay(speed - 20);
    }
    is_jumping = 0;
    jump = 0;
}

void game_loop() {
    load_high_score();
    last_score_time = clock();
    int base_speed = 90;
    int speed_increase_interval = 50;
    int speed_increase_amount = 30;
    while (game_active) {
        clock_t current_time = clock();
        if ((current_time - last_score_time) > (CLOCKS_PER_SEC / 10)) {
            score++;
            last_score_time = current_time;
            speed = base_speed - (score / speed_increase_interval) * speed_increase_amount;
            if (speed < 20) speed = 20;
        }
        clear_screen();
        Score();
        draw_environment();
        dinosour();
        move_obstacle();
        if (check_collision()) {
            game_over();
            break;
        }
        if (kbhit()) {
            int A = getch();
            if (A == ' ' && !is_jumping) {
                jump_animation();
            } else if (A == 27) {
                exit(0);
            }
        }
        delay(speed);
    }
    getch();
    y = 15;
    score = 0;
    speed = 90;
    obstacle_x = 80;
    game_active = 1;
}

void change_skin() {
    int option;
    system("cls");
    printf("Choose a Dino Color:\n");
    printf("1. White\n");
    printf("2. Light Green\n");
    printf("3. Light Red\n");
    printf("4. Yellow\n");
    printf("5. Cyan\n");
    printf("Choice: ");
    scanf("%d", &option);

    switch(option) {
        case 1: dino_color = 15; break; 
        case 2: dino_color = 10; break; 
        case 3: dino_color = 12; break; 
        case 4: dino_color = 14; break; 
        case 5: dino_color = 11; break; 
        default: printf("Invalid choice. Using default.\n"); dino_color = 15;
    }

    printf("Dino skin updated!\n");
    Sleep(1000);
}

void main_menu() {
    load_leaderboard();
    while (1) {
        clear_screen();
        printf("===== DINOSAUR GAME =====\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. View Leaderboard\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                if (login()) {
                    while (1) {
                        clear_screen();
                        printf("===== WELCOME %s =====\n", current_user.username);
                        printf("High Score: %d\n", current_user.high_score);
                        printf("1. Play Game\n");
						printf("2. View Leaderboard\n");
						printf("3. Logout\n");
						printf("4. Change Skin\n"); 	
						printf("Choose an option: ");
                        int game_choice;
						scanf("%d", &game_choice);
						switch (game_choice) 
						{
						    case 1: game_loop(); break;
						    case 2: display_leaderboard(); break;
						    case 3: break; 
						    case 4: change_skin(); break;
						    default: printf("Invalid choice.\n");
						}
                    }
                }
                break;
            case 2:
                register_account();
                break;
            case 3:
                display_leaderboard();
                break;
            case 4:
                free_leaderboard();
                exit(0);
            default:
                printf("Invalid choice. Press any key to continue...\n");
                getch();
        }
    }
}

int main() {
    main_menu();
    return 0;
}
