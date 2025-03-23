#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<curses.h>
#include<string.h>
#include<time.h>

#define MAX_SCORE 500
#define FRAME_TIME 110000

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

int score = 0;
char score_message[16];

bool skip = false;
bool is_running = true;

int screen_width = 25;
int screen_height = 20;

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
int highest_score();
void init(); 
void process_input();
void restart_game();
void draw_border(int y, int x, int width, int height);
vec2 spawn_berry();
void draw();
void quit_game();
void game_over();
void guidelines();
void update();
bool collide(vec2 a, vec2 b);
bool collide_snake_body(vec2 point);

int main(){
    int choice = 0;
    record();
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
  while(true) {
    process_input();
    if (skip == true ) {
       skip = false;
       continue;
    }
    update();
    draw();
    }
    quit_game();
   
            break;
        
        case 2:
             printf("\nesjdfs");
             break;
         
        case 3:
        printf("\nhightest socre");
         // highest_score();
          break;


        case 4:
            guidelines();
        break;

        case 5:
           quit_game() ;
            break;
    }

;
  //  init();
  /*  while(true) {
    process_input();
    if (skip == true ) {
       skip = false;
       continue;
    }
    update();
    draw();
    }
    quit_game(); */
    return 0;
}

void record(){
     r a;
     int n;
     FILE *fp;
     fp = fopen("records.txt","w");
     printf("\n Enter your name: ");
     scanf("%s",a.name);
     fprintf(fp,"\nName = %s",a.name);
     
     //fp = fopen("records.txt","r");
    
     fclose(fp);  
}



int  highest_score() {
     FILE *fp;
     char ch;
     fp = fopen("records.txt","");
     if (fp == NULL){
        perror("\nError opening file");
        return 1;
     }
     printf("\n Contents of %s : ",record);
     while( (ch = fgetc(fp)) != EOF ){
          putchar(ch);
     }     
     fclose(fp);
     return 0;
}     

 
void guidelines() {
           printf("\n==============================HELP_GUIDE================================================");
           printf("\nWelcome to snake game");
           printf("\nTo play the snake game, use arrows keys to control the snake's direction, aim to eat food to grow longer, and avoid colliding with the boundaries or your own body, as the game ends when that happens.");
           printf("\n1.Movement:");
           printf("\nThe snake moves in one of four direactions (up,down,left,or right) based on the arrow key input.)");
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
        quit_game();
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

void restart_game() {
     head.x = 0;
     head.y = 0;
     dir.x = 0;
     dir.y = 0;
     score = 0;
     sprintf(score_message, "[ Score: %d ]",score);
     is_running = true;
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

           usleep(FRAME_TIME);
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

void draw() {
     int i = 0;
     erase();

     attron(COLOR_PAIR(1));
     mvaddch(berry.y+1, berry.x * 2+1,'@');
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

void quit_game() {
     // exit cleanly from application
     endwin();
     //clear screen, place cursor on top, and un-hide cursor
     printf("\e[1;1H\e[2J");
     printf("\e[?25h");

     exit(0);
}     

void game_over() {
     while (is_running == false) {
           process_input();

           mvaddstr(screen_height / 2, screen_width -16, "          Game Over       ");
           mvaddstr(screen_height / 2 + 1, screen_width - 16, "[SPACE] to restart, [ESC] to quit ");
           attron(COLOR_PAIR(3));
           draw_border(screen_height / 2 - 1, screen_width - 17, 17, 2);
           attroff(COLOR_PAIR(3));

           usleep(FRAME_TIME);
      }
}      
















































