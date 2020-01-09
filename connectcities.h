/*
	header for connectcities v. 2.0
	github.com/return5
	license: MIT
*/


//----------------------------------headers ----------------------------------------
#include <ncurses.h>  //needs -lncurses gcc flag
#include <unistd.h>	 //usleep
#include <stdlib.h>	//malloc and realloc
#include <time.h>  //time()
#include <math.h>  //hypot
#include "pieces.h"

//---------------------------------- macros ----------------------------------------

#define HEIGHT 20
#define WIDTH 35
#define SLEEP_TIME 150000
#define CITY_LIMIT 11
#define CITY_MIN 8

//----------------------------------- prototypes -----------------------------------

void initScreen(void);
void initWindows(void);
void initColors(void);
void makeCITIES(void);
void gameLoop(void);
CITY *makeNewCity(const int i);
enum COLORS getCityColor(void);
int checkIfSameXAndY(const int x, const int y,const int i);
void getUserInput(void);
void printCities(void);
void moveY(const int y_dir);
void moveX(const int x_dir);
void setPrev(void);
void moveCurs(void);
void initGame(void);
int checkIfCursCity(void);
void userPressY(void);
TRACKPIECE *makeNewTrackPiece(void);
void buildNextTrackPiece(void);
TRAINTRACK *makeNewTrainTrack(const int i);
void makeNewTrack(const int i);
void moveXY(const int x_dir, const int y_dir);
void printToPrompt(const char *const str);
void updateBoard(void);
MOVETRAIN *makeMoveTrain(void);
void makeNewTrain(TRAINTRACK *const traintrack);
int checkIfUnfinishedTrack(void);
TRACKPIECE *getLeftTrack(const TRACKPIECE *const track);
TRACKPIECE *getRightTrack(const TRACKPIECE *const track);
int checkRight(const TRAINTRACK *const traintrack) ;
int checkLeft(const TRAINTRACK *const traintrack);
int moveTrain(const TRAINTRACK *const traintrack);
char getTrainPiece(const TRAIN *const train,const TRACKPIECE *const next_loc);
void moveAllTrains(void);
void makeEndTrack(const int i);
void moveCar(TRAIN *const train,TRACKPIECE *const next_loc);
int checkIfCity(const TRAINTRACK *const traintrack, const TRACKPIECE *const newloc);
void swapDirections(void);
char getCaboosePiece(const TRACKPIECE *const current_loc,const TRACKPIECE *const next_loc);
char getEnginePiece(const TRACKPIECE *const current_loc,const TRACKPIECE *const next_loc);
double makePayout(void);
void payOutMonies(void);
void userPressG(void);
void moveTrainCarsAlongTrack(TRAIN *headtrain, MOVETRAIN *trainmove, TRACKPIECE *newloc);
void printPlayerMoney(void);
int getFinalScore(void);
void endGame(void);
void userPressQ(void);
