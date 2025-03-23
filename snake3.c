#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<curses.h>
#include<string.h>
#include<time.h>

#define MAX_SCORE 500
#define EASY_SPEED 200000
#define MEDIUM_SPEED 110000
#define HARD_SPEED 50000

struct vec2 {
      int x;
      int y;
};

typedef struct vec2 vec2;

struct record{
       int score;
       char name[20];
};

typedef struct record r;

//Global variables
int score = 0;
char score_message[16];

bool skip = false;
bool is_running = true;
bool name_entered = false;

int screen_width = 50;
int screen_height = 50;

int snake_speed = MEDIUM_SPEED;

//initalize screen
WINDOW *win;

//snake
vec2 head = {0, 0};
vec2 segments[MAX_SCORE + 1];
vec2 dir ={ 1, 0};

//berry
vec2 berry;

//Function decleration

void record();
void highest_score();
void guidelines();
void init();
void draw_border(int y, int x, int width, int height);
void draw();
void process_input();
bool collide(vec2 a, vec2 b);
bool collide_snake_body(vec2 point);
vec2 spawn_berry();
void update();
void restart_game();
void game_over();
void quit_game();

int main(){
    int choice = 0, level = 0;
    record();
    while(1) {
    printf("========== Snake game ============");
    printf("\n1. Start game");
    printf("\n2. Select level");
    printf("\n3. Highest score");
    printf("\n4. Help");
    printf("\n5. Exit");
    printf("\nEnter Your choice:");
    scanf("%d",&choice);;

    switch(choice){
       case 1:
            init();
            is_running = true;
            while(is_running) {
            process_input();
            if (skip ) {
            skip = false;
            continue;
         }
         update();
         draw();
         }
        game_over();
         break;
        
        case 2:
             printf("\nSelect Difficulty Level:");
             printf("\n1. Easy");
             printf("\n2. Medium");
             printf("\n3. Hard");
             printf("\nEnter Your Choice: ");
             scanf("%d",&level);

             if (level == 1) {
                 snake_speed = EASY_SPEED;
             }
             else if (level == 2) {
                  snake_speed = MEDIUM_SPEED;
             }
             else if (level == 3) {
                   snake_speed = HARD_SPEED;
             }
             else {
                 printf("\nInvalid choice! Defaulting to Medium.");
                 snake_speed = MEDIUM_SPEED;
             }

             printf("\nDifficulty set! Press 1 in the main menu to start.\n");
             printf("\n");
        break;
         
        case 3:
            highest_score();
            printf("\n");
        break;

        case 4:
            guidelines();
            printf("\n");
        break;

        case 5:
            quit_game() ;
        break;
     }
  }
}

void record(){
     static r a;
     FILE *fp;

     if (!name_entered) {
        printf("\n Enter your name: ");
        scanf("%s",a.name);
        name_entered = true;
     }

     a.score = score;

     fp = fopen("records.txt","a");
     if (fp == NULL) {
        perror("Error opening file");
        return;
     }

     fprintf(fp,"%s %d\n",a.name, a.score);
      
     fclose(fp);  
}

void  highest_score() {
     FILE *fp;
     char line[100];
    
     fp = fopen("records.txt","r");
     if (fp == NULL){
        perror("\nError opening file");
        return;
     }

     printf("\n====== Highest Score Record ======\n");
     while ( fgets (line, sizeof(line), fp)) {
          printf("%s", line);
     }     

     fclose(fp);
}     

void guidelines() {
           printf("\n==============================HELP_GUIDE================================================");
           printf("\nWelcome to snake game");
           printf("\nTo play the snake game, use arrows keys to control the snake's direction, aim to eat food to grow longer, and avoid colliding with the boundaries or your own body, as the game ends when that happens.");
           printf("\n1.Movement:");
           printf("\nThe snake moves in one of four directions (up,down,left,or right) based on the arrow key input.)");
           printf("\n1.Food:");
           printf("\nRandomly placed food on the grid serves as the snake objective, eating it causes the snake to grow longer.");
           printf("\n3.Game Over:");
           printf("\nThe game ends if the snake collides with the boundary of the screen or its own body.");
           printf("\n4.Objective: ");
           printf("\nThe goal is to make the snake as long as possible before the game ends.");
           printf("\n===============================THANK YOU================================================");

}
      
void init() { 
     srand(time(NULL));
     //initalize window
     win = initscr();
     //take player input and hide cursor
     keypad(win, true);
     noecho();
     nodelay(win, true);
     curs_set(0);

     //initalize color 
     if (has_colors() ==  FALSE) {
        endwin();
        fprintf(stderr, "Your terminal does not support color\n");
        exit(1);
     }
     start_color();
     use_default_colors();
     init_pair(1, COLOR_RED, -1);
     init_pair(2, COLOR_GREEN, -1);
     init_pair(3, COLOR_YELLOW, -1);

     berry.x = rand() % screen_width;
     berry.y = rand() % screen_height;

     //update score message
     sprintf(score_message, "[ Score: %d]",score);
}     

void draw_border(int y, int x, int width, int height) {
     int i = 0;
     //top row 
     mvaddch(y,x, ACS_ULCORNER);
     mvaddch(y, x + width * 2 + 1, ACS_URCORNER);
     for (i = 1; i < width * 2 + 1; i++) {
         mvaddch(y, x + i, ACS_HLINE);
     }

     //vertical lines
     for (i = 1; i < height + 1; i++) {
         mvaddch(y + i, x, ACS_VLINE);
         mvaddch(y + i, x + width * 2 + 1, ACS_VLINE);
     }

     // bottom row
     mvaddch(y + height + 1, x, ACS_LLCORNER);
     mvaddch(y + height + 1, x + width * 2 + 1, ACS_LRCORNER);
     for (i = 1; i < width * 2 + 1; i++) {
         mvaddch(y + height + 1, x + i, ACS_HLINE);
     }
}

void draw() {
     int i = 0;
     erase();

     attron(COLOR_PAIR(1));
     mvaddch(berry.y+1, berry.x * 2+1,'O');
     attroff(COLOR_PAIR(1));

     //draw snake
     attron(COLOR_PAIR(2));
     for (i = 0; i < score; i++) {
         mvaddch(segments[i].y+1, segments[i].x * 2 + 1, '█'); //ctrl+shift+u 2588
     }
     mvaddch(head.y+1, head.x * 2+1, '█');
     attroff(COLOR_PAIR(2));

     attron(COLOR_PAIR(3));
     draw_border(0, 0, screen_width, screen_height);
     attroff(COLOR_PAIR(3));
     mvaddstr(0, screen_width - 5, score_message);
}

void process_input() {
     int pressed = wgetch(win);
     if(pressed == KEY_LEFT ) {
         if(dir.x == 1) { 
             return;
             skip = true;
          }   
            dir.x = -1;
            dir.y = 0;
         } 
     if(pressed == KEY_RIGHT ) { 
         if(dir.x == -1) {
             return;
         }    
            dir.x = 1;
            dir.y = 0; 
         }   
     if(pressed == KEY_UP ) {
        if(dir.y == 1) {
            return ;
        }    
            dir.x = 0;
            dir.y = -1;
         }    
     if(pressed == KEY_DOWN ) {
        if(dir.y == -1) {
            return;
        }    
            dir.x = 0;
            dir.y = 1;
       }   
    if(pressed == ' ') {
        if(!is_running)
           restart_game();
     }
     if (pressed == '\e' ) {
        is_running = false;
        main();
     }
}     

bool collide(vec2 a, vec2 b) {
     if (a.x == b.x && a.y == b.y) {
        return true;
     }
     else return false;
}

bool collide_snake_body(vec2 point) {
     int i = 0;
     for (i = 0; i < score; i++) {
         if (collide(point, segments[i])) {
            return true;
         }
     }
     return false;
}

vec2 spawn_berry(){
     // spawn a new berry which is 1 pixel padding from edges and not inside oof the snake
     vec2 berry = { 1 + rand() % (screen_width - 2), 1 + rand() % (screen_height - 2)};
     while (collide(head,berry) || collide_snake_body(berry)) {
         berry.x = 1 + rand() % (screen_width - 2);
         berry.y = 1 + rand() % (screen_height - 2);
     }
     return berry;
}

void update() {
     //update snake segments
     int i = 0;
     for (i = score; i > 0; i--) {
              segments[i] = segments[i - 1];
          }
          segments[0] =  head;
          
          //move snake
          head.x += dir.x;
          head.y += dir.y;

          // collide with body or walls
          if (collide_snake_body(head) || head.x < 0 || head.y < 0 \
              || head.x >= screen_width || head.y >= screen_height) {
              is_running = false;
              game_over();
          } 

          // eating a berry
          if (collide(head, berry)) {
             if (score < MAX_SCORE) {
                score += 1;
                sprintf(score_message, "[ Score: %d ]",score);
             }
             else {
                //WIN!
                printf("YOU WIN!");
              }
              berry = spawn_berry();
           }

           usleep(snake_speed);
}

void restart_game() {
    int i = 0;
     head.x = 0;
     head.y = 0;
     dir.x = 1;
     dir.y = 0;
     score = 0;

     for (i = 0; i < MAX_SCORE; i++) {
          segments[i].x = -1;
          segments[i].y = -1;

     sprintf(score_message, "[ Score: %d ]",score);

     is_running = true;
     
     init();

     }
}
        
void game_over() {
     int key;
     record();
     nodelay(win, false);

     while (true) {
           erase();
           mvaddstr(screen_height / 2, screen_width -16, "          Game Over       ");
           mvaddstr(screen_height / 2 + 1, screen_width - 16, "[SPACE] to restart, [M] to menu ");

           attron(COLOR_PAIR(3));
           draw_border(screen_height / 2 - 1, screen_width - 17, 17, 2);
           attroff(COLOR_PAIR(3));
           
           refresh();

           key = wgetch(win);
           mvprintw(screen_height / 2 + 2, screen_width -16, "Pressed: %d",key);

           if (key == ' ') {
              mvprintw(screen_height / 2 + 3, screen_width -16, "Restarting...");
              refresh();
              usleep(500000);
              restart_game();
          /*    endwin();
              init();
              is_running = true; */
              return;
            }

            else if (key == 'm' || key == 'M' ) {
                endwin();
                main();
           }     
           usleep(snake_speed);
      }
}      


void quit_game() {
     // exit cleanly from application
     endwin();
     //clear screen, place cursor on top, and un-hide cursor
     printf("\e[1;1H\e[2J");
     printf("\e[?25h");

     exit(0);
}     









































