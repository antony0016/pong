#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "util.hpp"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int CENTER_WIDTH = 3;

const float MAX_ANGLE = 3.142 / 5.0; // 72 degrees

const SDL_Color whiteColor = {255, 255, 255};
const int MESSAGE_TIME = 40;
const int FONT_SIZE = 40;
const int PLAYER_SPEED = 10;

const float BALL_MAXSPEED = 30.0f;
const float BALL_ACCELERATE = 1.05f;
const float BALL_INIT_SPEED = 4.0f;
const int BALL_WIDTH = 10;
const int BALL_HEIGHT = 10;

typedef struct {
    SDL_Rect pos;
    int speed;
    int score;
} Player;

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float speed;
    float r = 1;
    void reset(){
        srand(time(NULL));
        x = SCREEN_WIDTH / 2;
        y = SCREEN_HEIGHT / 2;
        if(rand() % 2)
            r = -1;
        vx = 2 * r;
        vy = 2 * r;
        speed = 3;
    }
} Ball;

float calc_angle(float y1, float y2, int height) {
    float rely = y1 + height/2 - y2;
    rely /= height/2.0;   // Normalize
    return rely * MAX_ANGLE;
}

//Show message
void draw_text(int x,int y,SDL_Renderer *ren,string s){
    char buffer[100];
    sprintf(buffer, s.c_str());
    SDL_Texture *text = renderText(buffer, "fonts/sample.ttf", whiteColor, FONT_SIZE, ren);
    renderTexture(text, ren, x,y);
    SDL_DestroyTexture(text);
}

int main(int argc, char* argv[]) {
    cout << "Starting SDL Application..." << endl;
    SDL_Event e;
    SDL_Renderer *ren = nullptr;
    SDL_Window *win = nullptr;

    initialize(&ren, &win, SCREEN_WIDTH, SCREEN_HEIGHT);

    int board_width, board_height;
    SDL_Texture *squareTex = IMG_LoadTexture(ren, "img/pong_board.png"),*squareTex_1p = IMG_LoadTexture(ren, "img/pong_board.png")
		,*squareTex_2p = IMG_LoadTexture(ren, "img/pong_board.png");
    SDL_QueryTexture(squareTex, NULL, NULL, &board_width, &board_height);

    // Define players
    Player p1, p2;
	
    // x, y, w, h
    p1.pos = p2.pos = SDL_Rect{
      0,
      0,
      board_width,
      board_height,
    };

    p1.speed = p2.speed = PLAYER_SPEED;
    p1.score = p2.score = 0;

    p1.pos.x = board_width/2 + 10;
    p2.pos.x = SCREEN_WIDTH - p2.pos.w- 10 - p2.pos.w/2;

    p1.pos.y = SCREEN_HEIGHT/2 - p1.pos.h/2;
    p2.pos.y = SCREEN_HEIGHT/2 - p2.pos.h/2;

    // ------------------------------------
    // TODO: Define Ball
    Ball b;
    b.reset();
    // ------------------------------------

    cout << "Starting Game Loop" << endl;

    int crit_frame1 = 0,crit_frame2 = 0,point_frame = 0,p1_frame = 0,p2_frame = 0;
    bool quit = false,crit1 = false,crit2 = false,point1_bool = false,point2_bool = false;
    char buffer[512];
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

    unsigned int frame = 0;
    while(!quit) {
      while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT)
          quit = true;
        if(e.type == SDL_KEYDOWN) {
          switch(e.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
              quit = true;
              break;
          }
        }
      }
		
		
	  if(p1_frame == 20){
	  	squareTex_1p = IMG_LoadTexture(ren, "img/pong_board.png");
	  	p1_frame = 0;
	  }else{
	  	p1_frame++;
	  }
	  if(p1_frame == 20){
	  	squareTex_2p = IMG_LoadTexture(ren, "img/pong_board.png");
	  	p2_frame = 0;
	  }else{
	  	p2_frame++;
	  }
		
      // Player Movement
      if(keystates[SDL_SCANCODE_UP] && keystates[SDL_SCANCODE_C])
        p1.pos.y -= p1.speed / 2;
      else if(keystates[SDL_SCANCODE_UP] && keystates[SDL_SCANCODE_X])
        p1.pos.y -= p1.speed * 2;
      else if(keystates[SDL_SCANCODE_UP])
        p1.pos.y -= p1.speed;
      if(keystates[SDL_SCANCODE_DOWN] && keystates[SDL_SCANCODE_C])
        p1.pos.y += p1.speed / 2;
      else if(keystates[SDL_SCANCODE_DOWN] && keystates[SDL_SCANCODE_X])
        p1.pos.y += p1.speed * 2;
      else if(keystates[SDL_SCANCODE_DOWN])
        p1.pos.y += p1.speed;

      // ---------------------------------------
      // TODO: Player 2 controlled by human
      // Basic AI
      if(b.y < p2.pos.y + p2.pos.h/2 && b.x > SCREEN_WIDTH / 2)
        p2.pos.y -= p2.speed;

      if(b.y > p2.pos.y + p2.pos.h/2 && b.x > SCREEN_WIDTH / 2)
        p2.pos.y += p2.speed;
      // ---------------------------------------


      // Limit Ball Max Speed
      if(b.vx > BALL_MAXSPEED)
        b.vx = BALL_MAXSPEED;

      if(b.vy > BALL_MAXSPEED)
        b.vy = BALL_MAXSPEED;

      // Update Ball coordinates
      b.x += b.vx;
      b.y += b.vy;


      // ---------------------------------------
      // TODO: Boundary Collision of Ball
      // Y axis
      if (b.y < 0 || b.y + BALL_HEIGHT >= SCREEN_HEIGHT) {
        // Update ball's position and speed
        	b.vy *= -1;
      }
		
      // X axis
      if(b.x < 0) {
            p2.score++;
            point1_bool = true;
            b.reset();
        // Update ball's position and speed, and the player's score
      } else if(b.x + BALL_WIDTH >= SCREEN_WIDTH) {
            p1.score++;
            point2_bool = true;
            b.reset();
        // Update ball's position and speed, and the player's score
      }
      // ---------------------------------------

      // Boundary Collision of Players
      if(p1.pos.y < 0) p1.pos.y = 0;
      if(p1.pos.y + p1.pos.h > SCREEN_HEIGHT) p1.pos.y = SCREEN_HEIGHT - p1.pos.h;
      if(p2.pos.y < 0) p2.pos.y = 0;
      if(p2.pos.y + p2.pos.h > SCREEN_HEIGHT) p2.pos.y = SCREEN_HEIGHT - p2.pos.h;

      // Create a dummpy b_rect structure to detect collision
      SDL_Rect b_rect  = {
        (int) b.x,
        (int) b.y,
        BALL_WIDTH,
        BALL_HEIGHT,
      };

      // Player Collision
      if(SDL_HasIntersection(&p1.pos, &b_rect)) {
	   squareTex_1p = IMG_LoadTexture(ren, "img/pong_board1p.png");
        b.x = p1.pos.x + p1.pos.w;
        if(b.y > p1.pos.y + p1.pos.h / 2 - 20 && b.y < p1.pos.y + p1.pos.h / 2 + 10 && !crit1){
	   squareTex_1p = IMG_LoadTexture(ren, "img/pong_board1p_crit.png");
            b.speed += 0.5;
            crit1 = true;
        }
        b.speed = b.speed * BALL_ACCELERATE;
        float angle = calc_angle(p1.pos.y, b.y, p1.pos.h);
        b.vx = b.speed * cos(angle);
        b.vy = ((b.vy>0)? -1 : 1) * b.speed * sin(angle);
      } else if(SDL_HasIntersection(&p2.pos, &b_rect)) {
	squareTex_2p = IMG_LoadTexture(ren, "img/pong_board2p.png");
        if(b.y > p2.pos.y + p2.pos.h / 2 - 20 && b.y < p2.pos.y + p2.pos.h / 2 + 10 && !crit2){
	   squareTex_2p = IMG_LoadTexture(ren, "img/pong_board2p_crit.png");
            b.speed += 0.5;
            crit2 = true;
        }
        b.x = p2.pos.x - BALL_WIDTH;
        b.speed = b.speed * BALL_ACCELERATE;
        float angle = calc_angle(p2.pos.y, b.y, p2.pos.h);
        b.vx = -1 * b.speed * cos(angle);
        b.vy = ((b.vy>0)? -1 : 1) * b.speed * sin(angle);
      }

      SDL_RenderClear(ren);
		
	  // Draw the Player	
      SDL_RenderCopy(ren, squareTex_1p, NULL, &p1.pos);
      SDL_RenderCopy(ren, squareTex_2p, NULL, &p2.pos);

      // Draw the center line
      renderTexture(squareTex, ren, SCREEN_WIDTH/2 - CENTER_WIDTH/2, 0, CENTER_WIDTH, SCREEN_HEIGHT);

      // Draw the Ball
      renderTexture(squareTex, ren, b.x, b.y, BALL_WIDTH, BALL_HEIGHT);

      //Draw fail
      if(point1_bool){
          point_frame++;
          draw_text(50,50,ren,"FAIL><");
          if(point_frame >= MESSAGE_TIME){
                point_frame = 0;
                point1_bool = false;
          }
      }
      if(point2_bool){
          point_frame++;
          draw_text(500,50,ren,"FAIL><");
          if(point_frame >= MESSAGE_TIME){
                point_frame = 0;
                point2_bool = false;
          }
      }

      //Draw crit
        if(crit1){
            crit_frame1++;
            draw_text(p1.pos.x + p1.pos.w + 2,p1.pos.y + p1.pos.h - 20,ren,"SPEED UP!");
            if(crit_frame1 >= MESSAGE_TIME){
                crit1 = false;
                crit_frame1 = 0;
            }
        }
        if(crit2){
            crit_frame2++;
            draw_text(p2.pos.x - p2.pos.w - 165,p2.pos.y + p2.pos.h - 20,ren,"SPEED UP!");
            if(crit_frame2 >= MESSAGE_TIME){
                crit2 = false;
                crit_frame2 = 0;
            }
        }

      // Display the score
      sprintf(buffer, "%d", p1.score);
      SDL_Texture *p1score = renderText(buffer, "fonts/sample.ttf", whiteColor, FONT_SIZE, ren);
      sprintf(buffer, "%d", p2.score);
      SDL_Texture *p2score = renderText(buffer, "fonts/sample.ttf", whiteColor, FONT_SIZE, ren);

      int width;
      SDL_QueryTexture(p1score, NULL, NULL, &width, NULL);

      renderTexture(p1score, ren, SCREEN_WIDTH/2 - width - 10, 10);
      renderTexture(p2score, ren, SCREEN_WIDTH/2 + 10, 10);

      SDL_DestroyTexture(p1score);
      SDL_DestroyTexture(p2score);

      SDL_RenderPresent(ren);

      frame++;
    }


    SDL_DestroyTexture(squareTex);
	SDL_DestroyTexture(squareTex_1p);
	SDL_DestroyTexture(squareTex_2p);
    Cleanup(&ren, &win);
    return 0;
}

