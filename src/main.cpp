#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <SDL2_gfxPrimitives.h>
#include <SDL2_rotozoom.h>
#include "SDL.h"

using namespace std;

const int WIDTH = 400;
const int HEIGHT = 640;
const int FPS = 30;
const int PAIR_PIPES = 2;
const int RECT_WIDTH = 50;
const int MIN_HEIGHT = 150;
const int VAR_HEIGHT = 150;
const int DIST_PIPE = 250;
const int GROUND_HEIGHT = 50;
const int FLAP_HEIGHT = 20;
const int FLAP_WIDTH = 30;
const int FLAP_VX = 5;
const int D_WIDTH = WIDTH*2;
const double PIE = 3.141592653589;
const double FLAP_MAX_ROT_UP = PIE/4;
const double FLAP_MAX_ROT_DOWN = -PIE/4;
const double GRAV = 9.8/( FPS*1.0);
const Uint32 PIPE_COLOR = 0xFF555555;
const int START_X = 50;
const int START_Y = 80;
const int FLAP_UP = 10;
SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
SDL_Surface *surf = NULL;
SDL_Rect camera = {0,0,WIDTH,HEIGHT};
double FLAP_VY = 0;

bool running = true;

//vector2d class from http://www.terathon.com/code/vector2d.html

class Vector2D
{
    public:
    
        double   x;
        double   y;
        
        Vector2D() {}
        
        Vector2D(double r, double s)
        {
            x = r;
            y = s;
        }


        Vector2D& Set(double r, double s)
        {
            x = r;
            y = s;
            return (*this);
        }
        
        double& operator [](long k)
        {
            return ((&x)[k]);
        }
        
        const double& operator [](long k) const
        {
            return ((&x)[k]);
        }
        
        Vector2D& operator +=(const Vector2D& v)
        {
            x += v.x;
            y += v.y;
            return (*this);
        }
        
        Vector2D& operator -=(const Vector2D& v)
        {
            x -= v.x;
            y -= v.y;
            return (*this);
        }
        
        Vector2D& operator *=(double t)
        {
            x *= t;
            y *= t;
            return (*this);
        }
        
        Vector2D& operator /=(double t)
        {
            double f = 1.0F / t;
            x *= f;
            y *= f;
            return (*this);
        }
        
        Vector2D& operator &=(const Vector2D& v)
        {
            x *= v.x;
            y *= v.y;
            return (*this);
        }
        
        Vector2D operator -(void) const
        {
            return (Vector2D(-x, -y));
        }
        
        Vector2D operator +(const Vector2D& v) const
        {
            return (Vector2D(x + v.x, y + v.y));
        }
        
        Vector2D operator -(const Vector2D& v) const
        {
            return (Vector2D(x - v.x, y - v.y));
        }
        
        Vector2D operator *(double t) const
        {
            return (Vector2D(x * t, y * t));
        }
        
        Vector2D operator /(double t) const
        {
            double f = 1.0F / t;
            return (Vector2D(x * f, y * f));
        }
        
        double operator *(const Vector2D& v) const
        {
            return (x * v.x + y * v.y);
        }
        
        Vector2D operator &(const Vector2D& v) const
        {
            return (Vector2D(x * v.x, y * v.y));
        }
        
        bool operator ==(const Vector2D& v) const
        {
            return ((x == v.x) && (y == v.y));
        }
        
        bool operator !=(const Vector2D& v) const
        {
            return ((x != v.x) || (y != v.y));
        }
        
        Vector2D& Normalize(void)
        {
            return (*this /= sqrtf(x * x + y * y));
        }
        
        Vector2D& Rotate(double angle)
		{
			double s = sinf(angle);
			double c = cosf(angle);
    
			double nx = c * x - s * y;
			double ny = s * x + c * y;
    
			x = nx;
			y = ny;
    
			return (*this);
		}

		Vector2D* getNormal(){
			Vector2D temp(-y,x);
			return &temp;
		}

};


inline double Dot(const Vector2D& v1, const Vector2D& v2)
{
    return (v1 * v2);
}

class Point2D : public Vector2D
{
    public:
        
        Point2D() {}
        
        Point2D(double r, double s) : Vector2D(r, s) {}
        
        Point2D& operator =(const Vector2D& v)
        {
            x = v.x;
            y = v.y;
            return (*this);
        }
        
        Point2D& operator *=(double t)
        {
            x *= t;
            y *= t;
            return (*this);
        }
        
        Point2D& operator /=(double t)
        {
            double f = 1.0F / t;
            x *= f;
            y *= f;
            return (*this);
        }
        
        Point2D operator -(void) const
        {
            return (Point2D(-x, -y));
        }
        
        Point2D operator +(const Vector2D& v) const
        {
            return (Point2D(x + v.x, y + v.y));
        }
        
        Point2D operator -(const Vector2D& v) const
        {
            return (Point2D(x - v.x, y - v.y));
        }
        
        Vector2D operator -(const Point2D& p) const
        {
            return (Vector2D(x - p.x, y - p.y));
        }

        Point2D operator *(double t) const
        {
            return (Point2D(x * t, y * t));
        }
        
        Point2D operator /(double t) const
        {
            double f = 1.0F / t;
            return (Point2D(x * f, y * f));
        }

		Point2D proj(Vector2D pa){
			double projx = (x*pa.x + y*pa.y)/(pa.x*pa.x + pa.y*pa.y) * pa.x;
			double projy = (x*pa.x + y*pa.y)/(pa.x*pa.x + pa.y*pa.y) * pa.y;
			//Point2D = new Point2D();
			//Point2D tempin = Point2D(x,y);
			//Point2D temp = (Dot(tempin,pa)*pa)/(Dot(pa,pa));
			return Point2D(projx,projy);
			//return temp;
		}
		double squaredDist(Point2D other){
			return ( (x - other.x)*(x - other.x) + (y - other.y)*(y - other.y));
		}

		double vectorDist(){
			return (x*x + y*y);
		}

		Point2D min(Point2D other){
			if(vectorDist() < other.vectorDist())
				return Point2D(x,y);
			else
				return other;
		}

		Point2D max(Point2D other){
			if(vectorDist() > other.vectorDist())
				return Point2D(x,y);
			else
				return other;
		}	
};


inline Vector2D operator *(double t, const Vector2D& v)
{
    return (Vector2D(t * v.x, t * v.y));
}

inline Point2D operator *(double t, const Point2D& p)
{
    return (Point2D(t * p.x, t * p.y));
}


inline double Magnitude(const Vector2D& v)
{
    return (sqrtf(v.x * v.x + v.y * v.y));
}

inline double InverseMag(const Vector2D& v)
{
    return (1.0F / sqrtf(v.x * v.x + v.y * v.y));
}

inline double SquaredMag(const Vector2D& v)
{
    return (v.x * v.x + v.y * v.y);
}

//end vector2d class code


class Node{
	public:
		Point2D* pts;
		Uint32 color;
		
		Node() {
			pts = (Point2D*)malloc(sizeof(Point2D)*4);
			color = 0xFF0000FF;
		}
		Node(Point2D* points){
			pts = points;
			color = 0xFF0000FF;
		}

		void draw(){
			Sint16 tempx[4];// = {0,100,100,0};
			Sint16 tempy[4];// = {100,100,0,0};
			for(int i = 0; i < 4; i++){
				tempx[i] = (Sint16)pts[i].x;
				tempy[i] = (Sint16)pts[i].y;
				//cout << pts[i].x << " ";
				//cout << tempx[i] << " ";
			}
			//cout << endl;
			filledPolygonColor(ren,tempx,tempy,4,color);
			//cout << tempx[0] << " " << tempx[1] << " " << tempx[2] << " " << tempx[3] << endl;
		}
};

class Bird : public Node{
	public:
		double rot;

		Bird() : Node(){
			rot = 0;
		}
		Bird(Point2D* points) : Node(points){
			rot = 0;
		}
		void rotate(double r){
			rot += r;
			if(rot > FLAP_MAX_ROT_UP){
				rot = FLAP_MAX_ROT_UP;
				return;
			}else if(rot < FLAP_MAX_ROT_DOWN){
				rot = FLAP_MAX_ROT_DOWN;
				return;
			}

			Vector2D center = pts[2] - (pts[2] - pts[0])/2;

			for(int i = 0; i < 4; i++){
				Point2D temp = pts[i] - center;
				pts[i] = temp.Rotate(r) + center;
			}
		}
	

};

vector<Node> walls;
vector<Node> pipes;
Bird flappy;

int tick = 0;
int rottick = 0;
bool falling = true;
void moveDown(){
	FLAP_VY += GRAV;
	for(int i = 0; i < 4; i++){
		flappy.pts[i].y += FLAP_VY;
	}
	if(++rottick > 5){
		flappy.rotate(0.05);
		rottick = 0;
	}
}

void moveUp(){
	if(falling){
		FLAP_VY = 0;
		falling = false;
	}
	for(int i = 0; i < 4; i++){
		flappy.pts[i].y -= FLAP_UP;
		flappy.rotate(-0.03);
		
	}
	tick = 0;
	rottick = 0;
}

void moveForward(){
	if(falling){
		moveDown();
	}
	else{
		if(++tick > 3){
			falling =  true;
			tick = 0;
		}	
	}
	int max = 0;
	for(int i = 0; i < pipes.size() ; i+=2){
		if(pipes[i].pts[0].x > max)
			max = pipes[i].pts[0].x;
	}
	
	for(int i = 0; i < pipes.size(); i+=2){
		for(int j = 0; j < 4; j++){
			pipes[i].pts[j].x -= FLAP_VX;
			pipes[i+1].pts[j].x -= FLAP_VX;
		}

		if(pipes[i].pts[1].x <= 0){
			int varh = VAR_HEIGHT - rand()%VAR_HEIGHT;
			pipes[i].pts[0].x = pipes[i+1].pts[0].x = max + DIST_PIPE;
			pipes[i].pts[1].x = pipes[i+1].pts[1].x = max + DIST_PIPE + RECT_WIDTH;
			pipes[i].pts[2].x = pipes[i+1].pts[2].x = max + DIST_PIPE + RECT_WIDTH;
			pipes[i].pts[3].x = pipes[i+1].pts[3].x = max + DIST_PIPE;
			pipes[i].pts[2].y = varh + MIN_HEIGHT;
			pipes[i].pts[3].y = varh + MIN_HEIGHT;
			pipes[i+1].pts[0].y = VAR_HEIGHT + varh + MIN_HEIGHT; 
			pipes[i+1].pts[1].y = VAR_HEIGHT + varh + MIN_HEIGHT;

		}
	}

}


void render(){
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);
	for(int i = 0; i < walls.size(); i++){
		walls[i].draw();
	}
	for(int i = 0; i < pipes.size(); i++){
		pipes[i].draw();
	}
	flappy.draw();
	SDL_RenderPresent(ren);
}


bool collidePipes(Node &pipe){
	Vector2D axis[8];
	axis[0] = *(flappy.pts[0] - flappy.pts[3]).getNormal();
	for (int i = 3; i > 0 ; i--){ //axis for fappy
		axis[i] = *(flappy.pts[i] - flappy.pts[i-1]).getNormal();
	}
	axis[4] = *(pipe.pts[0] - pipe.pts[3]).getNormal();
	for (int i = 7; i > 4 ; i--){ //axis for pipez
		axis[i] = *(pipe.pts[i-4] - pipe.pts[i-5]).getNormal();
	}
	for(int i = 0; i < 8; i++){
		//project points of fappy bird to the axis in question DONE
		//get the min max of the points DONE
		//project points of pipez to the axis in question DONE
		//get the min max oof the points DONE
		// if minfap 
		Point2D flapProj[4];
		Point2D pipeProj[4];
		for(int j = 0; j < 4; j++){
			flapProj[j] = flappy.pts[j].proj(axis[i]);
			pipeProj[j] = pipe.pts[j].proj(axis[i]);
		}

		Point2D minFlap = flapProj[0];
		Point2D maxFlap = flapProj[0];
		//find the minmax point
		/*for(int j = 3; j > 0; j--){
			Point2D pair1 = flapProj[j];
			for(int k = j - 1; k >= 0; k--){
				Point2D pair2 = flapProj[k];
				if(pair1.squaredDist(pair2) > minFlap.squaredDist(maxFlap)){
					minFlap = pair1.min(pair2);
					maxFlap = pair1.max(pair2);
				}
			}
		}*/

		for(int j = 0; j < 4; j++){
			for(int k = j; k < 4; k++){
				if(j==k) continue;
				if(flapProj[j].squaredDist(flapProj[k]) > minFlap.squaredDist(maxFlap)){
					minFlap = flapProj[j].min(flapProj[k]);
					maxFlap = flapProj[j].max(flapProj[k]);
				}
			}	
		}
		Point2D minPipe = pipeProj[0];
		Point2D maxPipe = pipeProj[0];
		//find the minmax point
		/*for(int j = 3; j > 0; j--){
			Point2D pair1 = pipeProj[j];
			for(int k = j - 1; k >= 0; k--){
				Point2D pair2 = pipeProj[k];
				if(pair1.squaredDist(pair2) > minPipe.squaredDist(maxPipe)){
					minPipe = pair1.min(pair2);
					maxPipe = pair1.max(pair2);
				}
			}
		}*/

		for(int j = 0; j < 4; j++){
			for(int k = j; k < 4; k++){
				if(j==k) continue;
				if(pipeProj[j].squaredDist(pipeProj[k]) > minPipe.squaredDist(maxPipe)){
					minPipe = pipeProj[j].min(pipeProj[k]);
					maxPipe = pipeProj[j].max(pipeProj[k]);
				}
			}	
		}

		Vector2D va = maxFlap - minFlap;
		Vector2D vb = maxPipe - minPipe;

		//(minFlap.x < maxPipe.x && maxPipe.x < maxFlap.x) || (minPipe.x < maxFlap.x && maxFlap.x < maxPipe.x) 
		//cout << Dot(va,maxPipe) << " " << Dot(vb,maxFlap) << endl;
		if(Dot(va,maxPipe) == 0 || Dot(vb,maxFlap) == 0){
			//if(i == 0)
			cout << "pasok" << i << endl;
			//cout << "possible collision at pipe " << i << endl;
			continue;

		}
		else{

			//cout << "miss " << i << endl;
			//cout << "labas" << endl;
			return false;
		}
			
	}
	cout << "nagcollide" << endl;
	return true;
};

void collide(){
	for(int i = 0; i < pipes.size(); i++){
		if(collidePipes(pipes[i]))
			running = false;
	}
};


void initGame(){
	for(int i = 1; i <= PAIR_PIPES; i++){
		int varh = VAR_HEIGHT - rand()%VAR_HEIGHT;
		Point2D* pipeup = new Point2D[4];
		pipeup[0] = Point2D(DIST_PIPE*i,0);
		pipeup[1] = Point2D(DIST_PIPE*i + RECT_WIDTH,0);
		pipeup[2] = Point2D(DIST_PIPE*i + RECT_WIDTH,varh + MIN_HEIGHT);
		pipeup[3] = Point2D(DIST_PIPE*i,varh + MIN_HEIGHT);
		pipes.push_back(*new Node(pipeup));
		
		Point2D* pipedown = new Point2D[4];
		pipedown[0] = Point2D(DIST_PIPE*i, VAR_HEIGHT+ varh + MIN_HEIGHT );
		pipedown[1] = Point2D(DIST_PIPE*i + RECT_WIDTH, VAR_HEIGHT+ varh + MIN_HEIGHT);
		pipedown[2] = Point2D(DIST_PIPE*i + RECT_WIDTH, HEIGHT - GROUND_HEIGHT);
		pipedown[3] = Point2D(DIST_PIPE*i, HEIGHT - GROUND_HEIGHT);
		
		pipes.push_back(*new Node(pipedown));
	}

	Point2D* wallup = new Point2D[4];
	wallup[0] = Point2D(0,0);
	wallup[1] = Point2D(WIDTH,0);
	wallup[2] = Point2D(WIDTH,GROUND_HEIGHT);
	wallup[3] = Point2D(0,GROUND_HEIGHT);
	walls.push_back(*new Node(wallup));
	
	Point2D* walldown = new Point2D[4];
	walldown[0] = Point2D(0,HEIGHT-GROUND_HEIGHT);
	walldown[1] = Point2D(WIDTH,HEIGHT-GROUND_HEIGHT);
	walldown[2] = Point2D(WIDTH,HEIGHT);
	walldown[3] = Point2D(0,HEIGHT);
	walls.push_back(*new Node(walldown));

	Point2D* birdpoints = new Point2D[4];
	birdpoints[0] = Point2D(START_X,START_Y);
	birdpoints[1] = Point2D(START_X + FLAP_WIDTH,START_Y);
	birdpoints[2] = Point2D(START_X + FLAP_WIDTH,START_Y + FLAP_HEIGHT);
	birdpoints[3] = Point2D(START_X,START_Y+ FLAP_HEIGHT);
	flappy = *new Bird(birdpoints);
};

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
	else{
		surf = SDL_GetWindowSurface(win);
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		run = false;
	}
	return run;
}

void close(){
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

int main( int argc, char* args[] ){
	if(!init()) return 1;
	SDL_Event event;
	Uint32 start;
	SDL_Event ev;

	initGame();

	while (running) {
		start = SDL_GetTicks();
		if(SDL_PollEvent(&ev)){
			if(ev.type == SDL_KEYDOWN){
				if(ev.key.keysym.sym == SDLK_SPACE)
						moveUp();
			}
		}
		collide();
		moveForward();
		render();
		if(1000/FPS > (SDL_GetTicks() - start)){
			SDL_Delay(1000/FPS - (SDL_GetTicks() - start));
		}
	}
	close();
	return 0;
}

