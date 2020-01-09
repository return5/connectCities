/*
	connect cities v. 2.0
	github.com/return5
	license: MIT
	simple game running in terminal. objective is to connect the cities with train tacks. 
	see 'readme.txt' for controls and game info
*/

//----------------------------------headers ----------------------------------------
#include "connectcities.h"

//---------------------------------- global vars -----------------------------------
static const size_t SIZE_TRACKPIECE  =   sizeof(TRACKPIECE);
static const size_t SIZE_TRAINTRACK  =   sizeof(TRAINTRACK);
static const size_t SIZE_TRAIN       =   sizeof(TRAIN);
static const size_t SIZE_CITY        =   sizeof(CITY);
static const size_t SIZE_MT          =   sizeof(MOVETRAIN);

static WINDOW       *game_win,*prompt_win;
static int          PLAYER_MONEY,PLAY_GAME,PLACING_TRACK,X,Y,PREV_X,PREV_Y,NUM_CITIES;
static CITY         **CITIES;                       //array to hold CITIES
static TRAINTRACK   *TRACKS,*CURRENT_TRAINTRACK;  //head of linked list to hold all traintracks and the current most recently placed traintrack

//---------------------------------- code ------------------------------------------


void printCities(void) {
	for(int i = 0; i < NUM_CITIES; i++) {
		wattron(game_win,COLOR_PAIR(CITIES[i]->color));
		mvwprintw(game_win,CITIES[i]->y,CITIES[i]->x,"@");
		wattroff(game_win,COLOR_PAIR(CITIES[i]->color));
	}
	wrefresh(game_win);
}

void initScreen(void) {
	initscr();
	noecho();	                //dont display key strokes
	cbreak();	    		   //disable line buffering
	keypad(stdscr, TRUE);	  //enable keypad
	refresh();
}

void initWindows(void) {
	WINDOW *game_border = newwin(HEIGHT+2,WIDTH+2,0,0);
	wborder(game_border,'|','|','-', '-', '+', '+', '+', '+');
	WINDOW *prompt_border = newwin(3,27,0,WIDTH + 4);
	wborder(prompt_border,'|','|','-', '-', '+', '+', '+', '+');
	game_win = newwin(HEIGHT,WIDTH,1,1);
	prompt_win = newwin(1,25,1,WIDTH + 5);
	WINDOW *instruction_border = newwin(6,32,5,WIDTH + 4);
	wborder(instruction_border,'|','|','-', '-', '+', '+', '+', '+');
	WINDOW *instruction_win = newwin(4,30,6,WIDTH + 5);	
	wrefresh(instruction_win);
	wrefresh(prompt_win);
	wrefresh(prompt_border);
	wrefresh(game_win);	
	wrefresh(game_border);
	wrefresh(instruction_border);
	mvwprintw(instruction_win,0,0,"use arrow keys to move cursor");
	mvwprintw(instruction_win,1,0,"use y key to place track");
	mvwprintw(instruction_win,2,0,"use g key to start trains");
	mvwprintw(instruction_win,3,0,"use q key to quit");
	wrefresh(instruction_win);
}

void initColors(void) {
	start_color();
	init_pair(BLUE,COLOR_BLUE,COLOR_BLACK);
	init_pair(GREEN,COLOR_GREEN,COLOR_BLACK);
	init_pair(RED,COLOR_RED,COLOR_BLACK);
	init_pair(MAGENTA,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(YELLOW,COLOR_YELLOW,COLOR_BLACK);
	init_pair(CYAN,COLOR_CYAN,COLOR_BLACK);
}

void initGame(void) {
	PLACING_TRACK = 0;
	X = Y = PREV_X = PREV_Y = 0;
	PLAYER_MONEY = 200;
	PLAY_GAME = 1;
	printPlayerMoney();
}

void printPlayerMoney(void) {
	char c[25];
	sprintf(c,"player money: %d",PLAYER_MONEY);
	printToPrompt(c);
	moveCurs();
}

void printToPrompt(const char *const str) {
	wclear(prompt_win);
	mvwprintw(prompt_win,0,0,str);
	wrefresh(prompt_win);
}

enum COLORS getCityColor(void) {
	switch((rand() % 6)) {
		case 0: return BLUE;
		case 1: return GREEN;
		case 2: return RED;
		case 3: return MAGENTA;
		case 4: return YELLOW;
		case 5: return CYAN;
		default: return BLUE;
	}
}

//checks x and y coordinate for new city against all the other CITIES
int checkIfSameXAndY(const int x, const int y,const int i) { 
	for(int j = 0; j < i; j++) {
		//makes sure city is 6 blocks or more away from other CITIES
		if((x > CITIES[j]->x - 6 && x < CITIES[j]->x + 6) && (y > CITIES[j]->y - 6 && y < CITIES[j]->y + 6)) {
			return 1;
		}
	}
	return 0;
}

CITY *makeNewCity(const int i) {
	CITY *newcity = malloc(SIZE_CITY);
	do {
		newcity->x = rand() % WIDTH;    //set x to random value
		newcity->y = rand() % HEIGHT;  //set y to random value
	}while(checkIfSameXAndY(newcity->x,newcity->y,i)); //set new values if city is in same place as another
	newcity->color = getCityColor();
	return newcity;
}

void makeCITIES(void) {
	NUM_CITIES = (random() % (CITY_LIMIT - CITY_MIN + 1)) + CITY_MIN;  //set number of cities to random num between city_min and city_limit
	CITIES = malloc(SIZE_CITY * CITY_LIMIT);
	for(int i = 0; i < NUM_CITIES; i++) {
		CITIES[i] = makeNewCity(i);
	}
}

void setPrev(void) {
	PREV_X = X;
	PREV_Y = Y;
}

void moveCurs(void) {
	wmove(game_win,Y,X);
	wrefresh(game_win);
}

void moveX(const int x_dir) {
	if (x_dir == -1 && X >= 1) { //if moving down and not on bottom row
		X += x_dir;
	}
	else if (x_dir == 1 && X <= WIDTH - 2) { //if moving up and not on top row
		X += x_dir;
	} 
}

void moveY(const int y_dir) {
	if (y_dir == -1 && Y >= 1) { //if moving left and not on leftmost column
		Y += y_dir;
	}
	else if (y_dir == 1 && Y <= HEIGHT - 2) {  //if moving right and not on rightmost column
		Y += y_dir;
	} 
}

char getCaboosePiece(const TRACKPIECE *const current_loc,const TRACKPIECE *const next_loc) {
	if(current_loc->y > next_loc->y) {  //moving upward
		return 'v';
	}
	else if(current_loc->y < next_loc->y) {  //moving downward
		return '^';
	}
	else if(current_loc->x > next_loc->x) {  //moving left
		return '>';
	}
	else if(current_loc->x < next_loc->x) {  //moving right
		return '<';
	}
	return '<';  //default
}

char getEnginePiece(const TRACKPIECE *const current_loc,const TRACKPIECE *const next_loc) {
	if(current_loc->y > next_loc->y) {  //moving upward
		return '^';
	}
	else if(current_loc->y < next_loc->y) {  //moving downward
		return 'v';
	}
	else if(current_loc->x > next_loc->x) {  //moving left
		return '<';
	}
	else if(current_loc->x < next_loc->x) {  //moving right
		return '>';
	}
	return '>';  //default
}

//returns the char which will be printed to screen to represent the individual train cars
char getTrainPiece(const TRAIN *const train,const TRACKPIECE *const next_loc) {
	switch(train->piece) {
		case ENGINE: return getEnginePiece(train->location,next_loc);
		case CAR: return '+';
		case CABOOSE: return getCaboosePiece(train->location,next_loc);
		default: return '+';		
	}
}

TRACKPIECE *getLeftTrack(const TRACKPIECE *const track) {
	return track->prev;
}

TRACKPIECE *getRightTrack(const TRACKPIECE *const track) {
	return track->next;
}

int checkIfCity(const TRAINTRACK *const traintrack, const TRACKPIECE *const newloc) {	
	if(newloc->x == traintrack->head->x && newloc->y == traintrack->head->y ) {
		return 1;
	}
	if(newloc->x == traintrack->end->x && newloc->y == traintrack->end->y ) {
		return 2;
	}
	return 0;
}

void moveCar(TRAIN *const train,TRACKPIECE *const next_loc) {
	if(next_loc != NULL) {
		char c[2];
		sprintf(c,"%c",getTrainPiece(train,next_loc));
		train->location = next_loc;
		wattron(game_win,COLOR_PAIR(train->color));
		mvwprintw(game_win,train->location->y,train->location->x,c);
		wattroff(game_win,COLOR_PAIR(train->color));
	}
}

void moveTrainCarsAlongTrack(TRAIN *headtrain, MOVETRAIN *trainmove, TRACKPIECE *newloc) {
	while(headtrain != NULL && newloc != NULL) {
		moveCar(headtrain,newloc);
		newloc = trainmove->getNextTrack(newloc);
		headtrain = headtrain->next;
	}
	if(newloc != NULL) {
		mvwprintw(game_win,newloc->y,newloc->x,"#");
	}
}

//moves train down the track by one place. returns 0 when train reaches end of track. 
int moveTrain(const TRAINTRACK *const traintrack) {
	if(traintrack->movetrain->getNextTrack(traintrack->engine->location) != NULL || !checkIfCity(traintrack,traintrack->caboose->location)) {
		TRAIN *headtrain = traintrack->engine;
		while(traintrack->movetrain->getNextTrack(headtrain->location) == NULL) {
			headtrain = headtrain->next;
		}
		moveTrainCarsAlongTrack(headtrain,traintrack->movetrain->next,traintrack->movetrain->getNextTrack(headtrain->location));
		return 1;
	}
	return 0;
}

//move al trains down the tracks
void moveAllTrains(void) {
	int control = 1;
	curs_set(0);
	while(control) {
		control = 0;
		TRAINTRACK *head = TRACKS;
		while(head != NULL) {
			if(head->end != NULL) {  //if last_track connects to a city
				control += moveTrain(head);
			}
			head = head->next;		
		}
		printCities();
		usleep(SLEEP_TIME);
	}
	moveCurs();
	curs_set(1);
}

void swapDirections(void) {
	TRAINTRACK *head = TRACKS;
	while(head != NULL) {
		if(head->end != NULL) {
			head->movetrain = head->movetrain->next;
		}
		head = head->next;
	}
}

//get payout ratio by dividing distance between start and end points by length of track
double makePayout(void) {
	return hypot((CURRENT_TRAINTRACK->start_track->x - CURRENT_TRAINTRACK->last_track->x),(CURRENT_TRAINTRACK->start_track->y - CURRENT_TRAINTRACK->last_track->y)) / CURRENT_TRAINTRACK->len_track ;
}

//if track connects to city, then make it the end of that traintrack
void makeEndTrack(const int i) {	
	CURRENT_TRAINTRACK->end = CITIES[i]; //set end of track to selected city
	CURRENT_TRAINTRACK->start_track->prev = NULL;   //makes sure prev is null for first track
	CURRENT_TRAINTRACK->last_track->next = NULL;    //makes sure next is null for last track
	CURRENT_TRAINTRACK->movetrain = makeMoveTrain();
	PLACING_TRACK = 0;
	CURRENT_TRAINTRACK->payout = makePayout();
	curs_set(1);
	printCities();
	moveCurs();
}

void makeNewTrain(TRAINTRACK *const traintrack) {
	TRAIN *car;
	car = malloc(SIZE_TRAIN);  //make train engine
	car->piece = ENGINE;
	car->color = GREEN;
	car->location = traintrack->start_track;
	traintrack->engine = car;
	for(int i = 0; i < 3; i++) { //make 3 train cars
		car->next = malloc(SIZE_TRAIN);
		car = car->next;
		car->piece = CAR;
		car->color = YELLOW;
		car->location = traintrack->start_track;
	}
	car->next = malloc(SIZE_TRAIN); //make caboose
	car = car->next;
	car->piece = CABOOSE;
	car->color = RED;
	car->location = traintrack->start_track;
	traintrack->caboose = car;
}

//mkae a circluraly link list of MOVETRAIN
MOVETRAIN *makeMoveTrain(void) {
	MOVETRAIN *movetrain = malloc(SIZE_MT);
	movetrain->getNextTrack = getRightTrack;
	movetrain->next = malloc(SIZE_MT);
	movetrain->next->getNextTrack = getLeftTrack;
	movetrain->next->next = movetrain;  //set next to loop back to first movetrain
	return movetrain;
}

//makes a new trackpiece at current cursor position.
TRACKPIECE *makeNewTrackPiece(void) {
	TRACKPIECE *track = malloc(SIZE_TRACKPIECE);
	track->x = X;
	track->y = Y;
	PLAYER_MONEY -= 10;
	return track;
}

//builds the next piece along the track
void buildNextTrackPiece(void) {
	CURRENT_TRAINTRACK->last_track->next = makeNewTrackPiece(); //make new track and set it as last_piece
	CURRENT_TRAINTRACK->last_track->next->prev = CURRENT_TRAINTRACK->last_track;  //set new piece's previous to point to last_track
	CURRENT_TRAINTRACK->last_track = CURRENT_TRAINTRACK->last_track->next; //set last_track to recently made track	
	CURRENT_TRAINTRACK->len_track++;
	mvwprintw(game_win,Y,X,"#");
	wrefresh(game_win);
}

//makes new traintrack object. 
TRAINTRACK *makeNewTrainTrack(const int i) {
	TRAINTRACK *newtraintrack = malloc(SIZE_TRAINTRACK);
	newtraintrack->start_track = makeNewTrackPiece();
	newtraintrack->len_track = 1;
	newtraintrack->last_track = newtraintrack->start_track;
	newtraintrack->start_track->next = newtraintrack->last_track;
	newtraintrack->last_track->prev = newtraintrack->start_track;
	makeNewTrain(newtraintrack);
	newtraintrack->head = CITIES[i];
	return newtraintrack;
}

//make a new traintrack and place it at the end of the traintrack linked list
void makeNewTrack(const int i) {
	if(CURRENT_TRAINTRACK == NULL) {
		CURRENT_TRAINTRACK = makeNewTrainTrack(i);
		TRACKS = CURRENT_TRAINTRACK;
	}
	else {
		CURRENT_TRAINTRACK->next = makeNewTrainTrack(i);
		CURRENT_TRAINTRACK = CURRENT_TRAINTRACK->next;
	}
}

//checks if cursor is currently on a city,and if it is, then return the index to that city, otherwise return -1
int checkIfCursCity(void) {
	for(int i = 0; i <NUM_CITIES; i++) {
		if(CITIES[i]->x == X && CITIES[i]->y == Y) {
			return i;
		}
	}
	return -1;
}

//checks if cursor is on unfinished track
int checkIfUnfinishedTrack(void) {
	TRAINTRACK *head = TRACKS;
	while(head != NULL) {
		if(head->last_track->x == X && head->last_track->y == Y) {
			CURRENT_TRAINTRACK = head;
			return 1;
		}
		head = head->next;
	}
	return -1;
}

void payOutMonies(void) {
	TRAINTRACK *head = TRACKS;
	while (head != NULL) {
		if(head->end != NULL) {
			PLAYER_MONEY += (int)(head->len_track * head->payout * 10);
		}
		head = head->next;
	}
}

//if player moves cursor
void moveXY(const int x_dir, const int y_dir) {
	setPrev();
	moveX(x_dir);
	moveY(y_dir);
	moveCurs();
	if(PLACING_TRACK) {
		if(PLAYER_MONEY >= 10) {
			buildNextTrackPiece();
			int i;
			if((i = checkIfCursCity()) != -1 ) {
				makeEndTrack(i);
			}
		}
		else {
			printToPrompt("sorry, not enough monies");
			getch();
			PLACING_TRACK = 0;
			curs_set(1);
		}
		printPlayerMoney();
	}
}

//when user presses y, checks conditions to see if track can be built
void userPressY(void) {
	if(PLAYER_MONEY >= 10 && !PLACING_TRACK) {
		int i;
		if((i = checkIfCursCity()) != -1) {  //is player trying to start building a track starting at a city?
			PLACING_TRACK = 1;
			curs_set(0);  //turn off cursor
			makeNewTrack(i);
		}
		else if(checkIfUnfinishedTrack() != -1) {  //is player trying to finish building an unfinished track?		
			PLACING_TRACK = 1;
			curs_set(0);  //turn off cursor
		}
	} 
	else if(PLAYER_MONEY < 10 && !PLACING_TRACK){
		printToPrompt("sorry, not enough monies.");
		getch();
		printPlayerMoney();
	}
	else {
		PLACING_TRACK = 0;
		printCities();
		curs_set(1);  //turn on cursor
		moveCurs();
	}
	printPlayerMoney();
}

void userPressG(void) {
	if(!PLACING_TRACK) {
		moveAllTrains();
		swapDirections();
		payOutMonies();
		printPlayerMoney();
	}
}

void userPressQ(void) {
	printToPrompt("are you sure(y/n)?");
	if(getch() == 'y') {
		PLAY_GAME = 0;;
	}
	else {
	    printPlayerMoney();
    }
}

void getUserInput(void) {
	switch(getch()) {
		case KEY_UP: 
			moveXY(0,-1);
			break;
		case KEY_DOWN: 
			moveXY(0,1);
			break;	
		case KEY_LEFT: 
			moveXY(-1,0);
			break;	
		case KEY_RIGHT: 
			moveXY(1,0);
			break;	
		case 'y': userPressY();
			break;	
		case 'g': userPressG();
			break;	
		case 'q': userPressQ();
			break;	
		default:
			break;		
	}
}

int getFinalScore(void) {
	TRAINTRACK *head = TRACKS;
	double score = 0;
	while(head != NULL) {
		if(head->end != NULL) {
			score += head->payout;
		}
		head = head->next;
	}
	return (int) (PLAYER_MONEY * score);
}

void endGame(void) {
	curs_set(0);
	char c[30];
	sprintf(c,"your final score was %d",getFinalScore());
	wclear(game_win);
	mvwprintw(game_win,2,2,"GAME OVER");
	mvwprintw(game_win,3,2,c);
	wrefresh(game_win);
	getch();
	curs_set(1);
	endwin(); 
}

void gameLoop(void) {
	while(PLAY_GAME) {
		getUserInput();
	}
	endGame(); 
}

int main(void) {
	srand(time(0));    //seed random number generator
	makeCITIES();
	initScreen();
	initColors();
	initWindows();
	initGame();
	printCities();
	moveCurs();
	gameLoop();
}
