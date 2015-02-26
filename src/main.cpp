
#include <iostream>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <sstream>
//#include <cmath>
#include <vector>
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 400;
const int FPS = 60;
const int PAIR_PIPES = 3;
const int RECT_WIDTH = 50;
const int MIN_HEIGHT = 100;
const int VAR_HEIGHT = 50;
const int DIST_PIPE = 200;
const int GROUND_HEIGHT = 100;
const int FAP_SIZE = 25;
const int D_WIDTH = WIDTH*2;
const double PIE = 3.141592653589;
const double FAP_MAX_ROT_UP = PIE/4;
const double FAP_MAX_ROT_DOWN = -PIE/4;
const double GRAV = 10.0;
const int FAP_VX = 5;
const Uint32 PIPE_COLOR = 0xFF555555;
SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;



struct rekt{
	SDL_Rect rect;
	int cx, cy, h, w;
	rekt(){}
	rekt(int cenx, int ceny, int hei) : cx(cenx), cy(ceny),h(hei){
		rect.x = cenx - RECT_WIDTH/2;
		rect.y = ceny - hei/2;
		rect.w = RECT_WIDTH;
		rect.h = hei;
	}

	rekt(int cenx, int ceny, int hei,int wid) : cx(cenx), cy(ceny),h(hei),w(wid){
		rect.x = cenx - RECT_WIDTH/2;
		rect.y = ceny - hei/2;
		rect.w = RECT_WIDTH;
		rect.h = hei;
	}
};



vector<rekt> pipes;
vector<SDL_Rect> walls;
rekt bird;

void inititalize(){
	for(int i = 1; i <= PAIR_PIPES; i++){
		int varh = VAR_HEIGHT - rand()%VAR_HEIGHT;
		int othervarh = VAR_HEIGHT - varh;
		pipes.push_back(*new rekt(DIST_PIPE*i + RECT_WIDTH/2, HEIGHT - (GROUND_HEIGHT + (MIN_HEIGHT + varh)/2), (MIN_HEIGHT + varh)));
		pipes.push_back(*new rekt(DIST_PIPE*i + RECT_WIDTH/2,(MIN_HEIGHT + othervarh)/2, (MIN_HEIGHT + othervarh)));
	}
	//walls.push_back(*new rekt(
}

void moveUp(){
	// apply impulse to the rectangle
	// would you need to check whether or not you have to cahnge the vxvy of the shit
}

void moveDown(){
	// use acceleration
}

void collide(){
}



bool init(){
	bool run = true;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		run = false;
	}
	win = SDL_CreateWindow("Fappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		run = false;
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		run = false;
	}
	//SDL_ShowCursor(SDL_DISABLE);
	return run;
}


void render(){
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 255,55, 55,55);	
	for(int i = 0; i < pipes.size(); i++){
		SDL_RenderDrawRect(ren,&pipes[i].rect);
	}
	SDL_RenderPresent(ren);
}



int main( int argc, char* args[] ){
	if(!init()) return 1;
	inititalize();
	bool running = true;
	Uint32 start;
	SDL_Event ev;

	while (running) {
		// time here
		start = SDL_GetTicks();
		render();
		if(1000/FPS > (SDL_GetTicks() - start)){
			SDL_Delay(1000/FPS - (SDL_GetTicks() - start));
		}
	}
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
